#!/usr/bin/env python3
"""Hardware-in-the-loop del AMPC CartPole con STM32N6570-DK.

La planta se integra en el PC y la acción de control se calcula en la STM32.
El paper denota el estado como x = [y, y_dot, alpha, alpha_dot]^T.
Por compatibilidad con la red entrenada y el protocolo binario, el orden interno
se conserva como [y, alpha, y_dot, alpha_dot].
"""

from __future__ import annotations

import argparse
import csv
import math
import struct
import sys
import time
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path

import numpy as np

try:
    import serial
except ImportError:  # Permite ejecutar --self-test sin hardware ni pyserial.
    serial = None

try:
    from scipy.integrate import odeint
except ImportError as exc:  # pragma: no cover
    raise SystemExit("Falta scipy. Ejecuta: pip install scipy") from exc


REQUEST = struct.Struct("<4sI4f")
RESPONSE = struct.Struct("<4sIffIIi")
REQUEST_MAGIC = b"MPC1"
RESPONSE_MAGIC = b"MPR1"

# Restricciones y criterio de éxito usados por Hose, Gräfe y Trimpe.
POSITION_GLOBAL_LIMIT_M = 0.4
POSITION_FINAL_LIMIT_M = 0.1
ANGLE_FINAL_LIMIT_RAD = 0.17
CONTROL_LIMIT_V = 9.0
FINAL_WINDOW_SAMPLES = 10


@dataclass(frozen=True)
class PlantParameters:
    g: float = 9.81
    m_add: float = 0.02
    m_cart: float = 0.506
    m_rod: float = 0.23
    length_rod: float = 0.6393037858519218
    j_motor_eq: float = 0.2153426947227305
    ab: float = -3.9610232930789304
    ac: float = 1.3002429170001302
    b_eq: float = 3.961023945944119
    b_p: float = 0.0002073799633807001


def wrap_to_pi(angle: float) -> float:
    return (angle + math.pi) % (2.0 * math.pi) - math.pi


def wrap_to_pi_array(angles: np.ndarray) -> np.ndarray:
    """Versión vectorizada del ajuste angular empleado por los autores."""
    values = np.asarray(angles, dtype=np.float64)
    return np.arctan2(np.sin(values), np.cos(values))


def check_constraints_original(states: np.ndarray) -> bool:
    """Comprueba la restricción global original |y| < 0.4 m."""
    array = np.asarray(states, dtype=np.float64)
    return bool(np.all(np.abs(array[:, 0]) < POSITION_GLOBAL_LIMIT_M))


def check_final_state_constraints_original(states: np.ndarray) -> bool:
    """Comprueba |y| < 0.1 m y |wrapToPi(alpha)| < 0.17 rad al final."""
    array = np.asarray(states, dtype=np.float64)
    final_window = array[-min(FINAL_WINDOW_SAMPLES, len(array)):]
    y_ok = np.all(np.abs(final_window[:, 0]) < POSITION_FINAL_LIMIT_M)
    alpha_error = wrap_to_pi_array(final_window[:, 1])
    alpha_ok = np.all(np.abs(alpha_error) < ANGLE_FINAL_LIMIT_RAD)
    return bool(y_ok and alpha_ok)


def check_control_constraints_original(controls: np.ndarray) -> bool:
    """Comprueba la saturación física original |u| <= 9 V."""
    values = np.asarray(controls, dtype=np.float64)
    return bool(np.all(np.abs(values) <= CONTROL_LIMIT_V))


def plant_derivative(state: np.ndarray, _t: float, control_v: float,
                     params: PlantParameters) -> np.ndarray:
    """Ecuaciones continuas de dynamic.py con parámetros optimizados."""
    _, alpha, y_dot, alpha_dot = state

    mass = params.m_add + params.m_rod
    center = (
        params.length_rod / 2.0 * params.m_rod
        + params.length_rod * params.m_add
    ) / mass
    j_rod = params.m_rod * params.length_rod**2 / 12.0
    inertia = j_rod + params.m_add * params.length_rod**2
    cart_mass = params.m_cart + params.j_motor_eq

    h1 = cart_mass + mass
    h2 = mass * center
    h4 = mass * center**2 + inertia
    h7 = mass * center * params.g

    force = params.ab * y_dot + params.ac * control_v
    cos_alpha = math.cos(alpha)
    sin_alpha = math.sin(alpha)
    denominator = h2**2 * cos_alpha**2 - h1 * h4

    y_ddot = (
        h2 * h4 * alpha_dot**2 * sin_alpha
        - h2 * h7 * cos_alpha * sin_alpha
        + h4 * (force - params.b_eq * y_dot)
        - h2 * cos_alpha * params.b_p * alpha_dot
    ) / (-denominator)

    alpha_ddot = (
        h2**2 * alpha_dot**2 * cos_alpha * sin_alpha
        - h1 * h7 * sin_alpha
        + h2 * cos_alpha * (force - params.b_eq * y_dot)
        + h1 * params.b_p * alpha_dot
    ) / denominator

    return np.asarray([y_dot, alpha_dot, y_ddot, alpha_ddot], dtype=np.float64)


def integrate_plant(state: np.ndarray, control_v: float, dt: float,
                    params: PlantParameters) -> np.ndarray:
    """Replica el integrador scipy.odeint del simulador original."""
    trajectory = odeint(
        plant_derivative,
        state,
        np.asarray([0.0, dt], dtype=np.float64),
        args=(float(control_v), params),
    )
    next_state = np.asarray(trajectory[-1], dtype=np.float64)
    if not np.all(np.isfinite(next_state)):
        raise FloatingPointError("La planta produjo un estado no finito")
    return next_state


def read_exact(port: serial.Serial, size: int, timeout_s: float) -> bytes:
    deadline = time.monotonic() + timeout_s
    data = bytearray()
    while len(data) < size:
        if time.monotonic() >= deadline:
            raise TimeoutError(
                f"Respuesta UART incompleta: {len(data)}/{size} bytes"
            )
        chunk = port.read(size - len(data))
        if chunk:
            data.extend(chunk)
    return bytes(data)


def wait_ready(port: serial.Serial, timeout_s: float = 2.0) -> None:
    """Consume el aviso ASCII de arranque si sigue presente en el VCP."""
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        line = port.readline()
        if not line:
            continue
        text = line.decode("ascii", errors="replace").strip()
        if text:
            print(f"STM32> {text}")
        if text.startswith("READY,MPC_HIL,"):
            return
    print("Aviso READY no observado; se intentará comunicar igualmente.")


def request_control(port: serial.Serial, sample_id: int, state: np.ndarray,
                    timeout_s: float) -> tuple[float, float, int, int, float]:
    request = REQUEST.pack(
        REQUEST_MAGIC,
        sample_id,
        float(state[0]),
        # Propuesto: transmitir el ángulo continuo
        float(state[1]),
        #float(wrap_to_pi(float(state[1]))),
        float(state[2]),
        float(state[3]),
    )

    start_ns = time.perf_counter_ns()
    port.write(request)
    port.flush()
    raw = read_exact(port, RESPONSE.size, timeout_s)
    end_ns = time.perf_counter_ns()

    magic, response_id, control_v, output_norm, inference_us, cycles, status = (
        RESPONSE.unpack(raw)
    )
    if magic != RESPONSE_MAGIC:
        raise RuntimeError(f"Magic de respuesta inválido: {magic!r}")
    if response_id != sample_id:
        raise RuntimeError(
            f"ID fuera de secuencia: enviado={sample_id}, recibido={response_id}"
        )
    if status != 0:
        raise RuntimeError(f"La STM32 informó status={status}")

    rtt_us = (end_ns - start_ns) / 1000.0
    return float(control_v), float(output_norm), inference_us, cycles, rtt_us


class LivePlot:
    def __init__(self, dt: float, steps: int) -> None:
        try:
            import matplotlib.pyplot as plt
        except ImportError as exc:  # pragma: no cover
            raise SystemExit("Falta matplotlib. Ejecuta: pip install matplotlib") from exc

        self.plt = plt
        plt.ion()
        self.fig, self.axes = plt.subplots(5, 1, figsize=(12, 13.5), sharex=True)
        self.fig.suptitle(
            "Hardware-in-the-Loop del AMPC adaptativo - STM32N6570-DK",
            fontsize=14,
            y=0.995,
        )

        total_time = steps * dt
        final_window_start = max(
            0.0, (steps + 1 - FINAL_WINDOW_SAMPLES) * dt
        )

        self.lines = {
            "y": self.axes[0].plot([], [], label=r"$y$ [m]")[0],
            "alpha_error_state": self.axes[1].plot(
                [], [], label=r"$e_{\alpha}=\mathrm{wrapToPi}(\alpha)$ [rad]"
            )[0],
            "y_dot": self.axes[2].plot([], [], label=r"$\dot{y}$ [m/s]")[0],
            "alpha_dot": self.axes[2].plot(
                [], [], label=r"$\dot{\alpha}$ [rad/s]"
            )[0],
            "u": self.axes[3].step(
                [], [], where="post", label=r"$u$ [V]"
            )[0],
            "alpha_cont": self.axes[4].plot(
                [], [], label=r"$\alpha$ continuo [rad]"
            )[0],
            "alpha_error": self.axes[4].plot(
                [], [], label=r"$e_{\alpha}=\mathrm{wrapToPi}(\alpha)$ [rad]"
            )[0],
        }
        self.axes[0].set_title(r"Posición del carro ($y$) y restricciones")
        self.axes[1].set_title(
            r"Error angular del péndulo ($e_{\alpha}$) y criterio final"
        )
        self.axes[2].set_title(
            r"Velocidad del carro ($\dot{y}$) y velocidad angular ($\dot{\alpha}$)"
        )
        self.axes[3].set_title(r"Acción de control discreta ($u$)")
        self.axes[4].set_title(
            r"Ángulo continuo ($\alpha$) y error angular ($e_{\alpha}$)"
        )
        self.axes[0].set_ylabel(r"$y$ [m]")
        self.axes[1].set_ylabel(r"$e_{\alpha}$ [rad]")
        self.axes[2].set_ylabel(r"$\dot{y}$ [m/s] / $\dot{\alpha}$ [rad/s]")
        self.axes[3].set_ylabel("Tensión [V]")
        self.axes[4].set_ylabel("Ángulo [rad]")

        self.axes[0].axhline(
            POSITION_GLOBAL_LIMIT_M, color="tab:red", linestyle=":",
            label=r"Restricción global $|y|<0.4$ m"
        )
        self.axes[0].axhline(
            -POSITION_GLOBAL_LIMIT_M, color="tab:red", linestyle=":"
        )
        self.axes[0].axhline(
            POSITION_FINAL_LIMIT_M, color="tab:green", linestyle="--",
            label=r"Criterio final $|y|<0.1$ m"
        )
        self.axes[0].axhline(
            -POSITION_FINAL_LIMIT_M, color="tab:green", linestyle="--"
        )

        self.axes[1].axhline(
            ANGLE_FINAL_LIMIT_RAD, color="tab:green", linestyle="--",
            label=r"Criterio final $|e_{\alpha}|<0.17$ rad"
        )
        self.axes[1].axhline(
            -ANGLE_FINAL_LIMIT_RAD, color="tab:green", linestyle="--"
        )

        for axis in (self.axes[0], self.axes[1]):
            axis.axvline(
                final_window_start, color="tab:purple", linestyle="-.",
                label="Inicio de las últimas 10 muestras"
            )
            axis.axvspan(
                final_window_start, total_time, color="tab:purple", alpha=0.06
            )

        self.axes[3].axhline(
            CONTROL_LIMIT_V, color="tab:red", linestyle=":",
            label=r"Restricción $u=+9$ V"
        )
        self.axes[3].axhline(
            -CONTROL_LIMIT_V, color="tab:red", linestyle=":",
            label=r"Restricción $u=-9$ V"
        )
        self.axes[4].axhline(
            0.0, color="black", linewidth=1.0, linestyle="--",
            label="Equilibrio vertical"
        )
        for axis in self.axes:
            axis.grid(True, alpha=0.3)
            axis.legend(loc="upper right")
        self.axes[4].set_xlabel("Tiempo [s]")
        self.fig.tight_layout(rect=(0.0, 0.0, 1.0, 0.965), h_pad=1.25)

    def update(self, t: list[float], states: list[np.ndarray],
        controls: list[float]) -> None:
        array = np.asarray(states)
        self.lines["y"].set_data(t, array[:, 0])
        alpha_continuous = array[:, 1]
        alpha_error = wrap_to_pi_array(alpha_continuous)
        self.lines["alpha_error_state"].set_data(t, alpha_error)
        self.lines["y_dot"].set_data(t, array[:, 2])
        self.lines["alpha_dot"].set_data(t, array[:, 3])

        if controls:
            control_t = np.asarray(t[:len(controls) + 1])
            held_controls = np.append(np.asarray(controls), controls[-1])
            self.lines["u"].set_data(control_t, held_controls)
        self.lines["alpha_cont"].set_data(t, alpha_continuous)
        self.lines["alpha_error"].set_data(t, alpha_error)

        for axis in self.axes:
            axis.relim()
            axis.autoscale_view()
        self.fig.canvas.draw_idle()
        self.fig.canvas.flush_events()
        self.plt.pause(0.001)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Simula la planta CartPole y usa la STM32 como controlador AMPC"
    )
    parser.add_argument("--port", default="COM4", help="Puerto VCP, por defecto COM4")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--dt", type=float, default=0.160,
                        help="Periodo del simulador original, en segundos")
    parser.add_argument("--duration", type=float, default=16.0)
    parser.add_argument("--timeout", type=float, default=1.0)
    parser.add_argument("--y0", "--x0", dest="y0", type=float, default=0.0,
                        help="Posición inicial del carro y [m]")
    parser.add_argument("--alpha0", "--theta0", dest="alpha0", type=float,
                        default=0.30, help="Ángulo inicial alpha [rad]")
    parser.add_argument("--ydot0", "--v0", dest="ydot0", type=float,
                        default=0.0, help="Velocidad inicial del carro y_dot [m/s]")
    parser.add_argument("--alphadot0", "--dtheta0", dest="alphadot0",
                        type=float, default=0.0,
                        help="Velocidad angular inicial alpha_dot [rad/s]")
    parser.add_argument("--csv", type=Path, default=None)
    parser.add_argument("--no-plot", action="store_true")
    parser.add_argument("--no-realtime", action="store_true")
    parser.add_argument("--self-test", action="store_true")
    return parser.parse_args()


def self_test() -> int:
    assert REQUEST.size == 24
    assert RESPONSE.size == 28
    state = np.asarray([0.0, 0.3, 0.0, 0.0], dtype=np.float64)
    derivative = plant_derivative(state, 0.0, 0.0, PlantParameters())
    next_state = integrate_plant(state, 0.0, 0.160, PlantParameters())
    assert derivative.shape == (4,) and np.all(np.isfinite(derivative))
    assert next_state.shape == (4,) and np.all(np.isfinite(next_state))
    print("Self-test OK")
    print("dx/dt:", derivative)
    print("state(t+0.160):", next_state)
    return 0


def run(args: argparse.Namespace) -> int:
    if serial is None:
        raise RuntimeError("Falta pyserial. Ejecuta: pip install pyserial")

    dt = float(args.dt)
    if dt <= 0.0 or args.duration <= 0.0:
        raise ValueError("dt y duration deben ser positivos")

    steps = int(round(args.duration / dt))
    state = np.asarray(
        [args.y0, args.alpha0, args.ydot0, args.alphadot0], dtype=np.float64
    )
    params = PlantParameters()
    csv_path = args.csv or Path(
        f"hil_mpc_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
    )

    times: list[float] = [0.0]
    states: list[np.ndarray] = [state.copy()]
    controls: list[float] = []
    inference_ms: list[float] = []
    rtt_ms: list[float] = []
    missed_rtt_deadlines = 0
    late_pc_cycles = 0
    plot = None if args.no_plot else LivePlot(dt, steps)

    with serial.Serial(args.port, args.baud, timeout=0.05, write_timeout=1.0) as port, \
            csv_path.open("w", newline="", encoding="utf-8") as csv_file:
        wait_ready(port)
        writer = csv.writer(csv_file)
        writer.writerow([
            "id", "t_s", "y_m", "alpha_cont_rad", "alpha_error_rad",
            "y_dot_m_s", "alpha_dot_rad_s", "u_v", "output_normalized",
            "inference_us", "inference_cycles", "rtt_us", "deadline_missed"
        ])

        wall_start = time.perf_counter()
        for sample_id in range(steps):
            control_v, output_norm, infer_us, cycles, roundtrip_us = request_control(
                port, sample_id, state, args.timeout
            )
            missed = roundtrip_us > dt * 1_000_000.0
            missed_rtt_deadlines += int(missed)
            alpha_error = wrap_to_pi(float(state[1]))

            writer.writerow([
                sample_id, sample_id * dt, state[0], state[1], alpha_error,
                state[2], state[3], control_v, output_norm, infer_us, cycles,
                roundtrip_us, int(missed)
            ])
            csv_file.flush()

            controls.append(control_v)
            inference_ms.append(infer_us / 1000.0)
            rtt_ms.append(roundtrip_us / 1000.0)
            state = integrate_plant(state, control_v, dt, params)
            times.append((sample_id + 1) * dt)
            states.append(state.copy())

            if plot is not None:
                plot.update(times, states, controls)

            print(
                f"[{sample_id + 1:03d}/{steps:03d}] "
                f"y={state[0]:+.5f} alpha={state[1]:+.5f} "
                f"e_alpha={wrap_to_pi(float(state[1])):+.5f} "
                f"u={control_v:+.5f} V infer={infer_us} us "
                f"RTT={roundtrip_us:.1f} us"
            )

            if not args.no_realtime:
                next_deadline = wall_start + (sample_id + 1) * dt
                remaining = next_deadline - time.perf_counter()
                if remaining > 0.0:
                    time.sleep(remaining)
                else:
                    late_pc_cycles += 1

    print(f"CSV guardado: {csv_path.resolve()}")
    print(f"Deadlines RTT perdidos: {missed_rtt_deadlines}")
    print(f"Ciclos tardíos del PC (incluye gráfico): {late_pc_cycles}")
    states_array = np.asarray(states)
    controls_array = np.asarray(controls)
    global_ok = check_constraints_original(states_array)
    control_ok = check_control_constraints_original(controls_array)
    final_ok = check_final_state_constraints_original(states_array)
    print("=" * 60)
    print("CRITERIO ORIGINAL DE LOS AUTORES")
    print("=" * 60)
    print(f"Restricción global |y| < 0.4 m: {global_ok}")
    print(f"Restricción de control |u| <= 9 V: {control_ok}")
    print(f"Últimas 10: |y| < 0.1 m y |e_alpha| < 0.17 rad: {final_ok}")
    print(f"ÉXITO: {global_ok and control_ok and final_ok}")
    if inference_ms:
        print(
            "Inferencia [ms]: "
            f"media={np.mean(inference_ms):.4f}, "
            f"p99={np.percentile(inference_ms, 99):.4f}, "
            f"máx={np.max(inference_ms):.4f}"
        )
        print(
            "RTT [ms]: "
            f"media={np.mean(rtt_ms):.4f}, "
            f"p99={np.percentile(rtt_ms, 99):.4f}, "
            f"máx={np.max(rtt_ms):.4f}"
        )

    if plot is not None:
        plot.plt.ioff()
        plot.plt.show()
    return 0


def main() -> int:
    args = parse_args()
    if args.self_test:
        return self_test()
    try:
        return run(args)
    except KeyboardInterrupt:
        print("Ensayo interrumpido por el usuario.")
        return 130
    except Exception as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
