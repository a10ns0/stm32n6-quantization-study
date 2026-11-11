#!/usr/bin/env python3
"""Hardware-in-the-loop del AMPC CartPole con STM32N6570-DK.

La planta se integra en el PC y la acción de control se calcula en la STM32.
Orden de estado usado en todo el protocolo: [x, theta, v, dtheta].
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


def plant_derivative(state: np.ndarray, _t: float, control_v: float,
                     params: PlantParameters) -> np.ndarray:
    """Ecuaciones continuas de dynamic.py con parámetros optimizados."""
    _, theta, velocity, dtheta = state

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

    force = params.ab * velocity + params.ac * control_v
    cos_theta = math.cos(theta)
    sin_theta = math.sin(theta)
    denominator = h2**2 * cos_theta**2 - h1 * h4

    x_ddot = (
        h2 * h4 * dtheta**2 * sin_theta
        - h2 * h7 * cos_theta * sin_theta
        + h4 * (force - params.b_eq * velocity)
        - h2 * cos_theta * params.b_p * dtheta
    ) / (-denominator)

    theta_ddot = (
        h2**2 * dtheta**2 * cos_theta * sin_theta
        - h1 * h7 * sin_theta
        + h2 * cos_theta * (force - params.b_eq * velocity)
        + h1 * params.b_p * dtheta
    ) / denominator

    return np.asarray([velocity, dtheta, x_ddot, theta_ddot], dtype=np.float64)


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
        float(wrap_to_pi(float(state[1]))),
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
    def __init__(self, dt: float) -> None:
        try:
            import matplotlib.pyplot as plt
        except ImportError as exc:  # pragma: no cover
            raise SystemExit("Falta matplotlib. Ejecuta: pip install matplotlib") from exc

        self.plt = plt
        plt.ion()
        self.fig, self.axes = plt.subplots(4, 1, figsize=(11, 9), sharex=True)
        self.fig.suptitle("HIL AMPC CartPole - STM32N6570-DK")

        self.lines = {
            "x": self.axes[0].plot([], [], label="x [m]")[0],
            "theta": self.axes[0].plot([], [], label="theta [rad]")[0],
            "v": self.axes[1].plot([], [], label="v [m/s]")[0],
            "dtheta": self.axes[1].plot([], [], label="dtheta [rad/s]")[0],
            "u": self.axes[2].plot([], [], label="u [V]")[0],
            "infer": self.axes[3].plot([], [], label="Inferencia [ms]")[0],
            "rtt": self.axes[3].plot([], [], label="RTT [ms]")[0],
        }
        self.axes[3].axhline(dt * 1000.0, color="red", linestyle="--",
                             label=f"Deadline {dt * 1000.0:.0f} ms")
        for axis in self.axes:
            axis.grid(True, alpha=0.3)
            axis.legend(loc="upper right")
        self.axes[3].set_xlabel("Tiempo [s]")
        self.fig.tight_layout()

    def update(self, t: list[float], states: list[np.ndarray], controls: list[float],
               inference_ms: list[float], rtt_ms: list[float]) -> None:
        array = np.asarray(states)
        self.lines["x"].set_data(t, array[:, 0])
        self.lines["theta"].set_data(t, array[:, 1])
        self.lines["v"].set_data(t, array[:, 2])
        self.lines["dtheta"].set_data(t, array[:, 3])

        control_t = t[:len(controls)]
        self.lines["u"].set_data(control_t, controls)
        self.lines["infer"].set_data(control_t, inference_ms)
        self.lines["rtt"].set_data(control_t, rtt_ms)

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
    parser.add_argument("--x0", type=float, default=0.0)
    parser.add_argument("--theta0", type=float, default=0.30)
    parser.add_argument("--v0", type=float, default=0.0)
    parser.add_argument("--dtheta0", type=float, default=0.0)
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
    print("x(t+0.160):", next_state)
    return 0


def run(args: argparse.Namespace) -> int:
    if serial is None:
        raise RuntimeError("Falta pyserial. Ejecuta: pip install pyserial")

    dt = float(args.dt)
    if dt <= 0.0 or args.duration <= 0.0:
        raise ValueError("dt y duration deben ser positivos")

    steps = int(round(args.duration / dt))
    state = np.asarray(
        [args.x0, args.theta0, args.v0, args.dtheta0], dtype=np.float64
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
    missed_deadlines = 0
    plot = None if args.no_plot else LivePlot(dt)

    with serial.Serial(args.port, args.baud, timeout=0.05, write_timeout=1.0) as port, \
            csv_path.open("w", newline="", encoding="utf-8") as csv_file:
        wait_ready(port)
        writer = csv.writer(csv_file)
        writer.writerow([
            "id", "t_s", "x_m", "theta_rad", "v_m_s", "dtheta_rad_s",
            "u_v", "output_normalized", "inference_us", "inference_cycles",
            "rtt_us", "deadline_missed"
        ])

        wall_start = time.perf_counter()
        for sample_id in range(steps):
            control_v, output_norm, infer_us, cycles, roundtrip_us = request_control(
                port, sample_id, state, args.timeout
            )
            missed = roundtrip_us > dt * 1_000_000.0
            missed_deadlines += int(missed)

            writer.writerow([
                sample_id, sample_id * dt, *state.tolist(), control_v, output_norm,
                infer_us, cycles, roundtrip_us, int(missed)
            ])
            csv_file.flush()

            controls.append(control_v)
            inference_ms.append(infer_us / 1000.0)
            rtt_ms.append(roundtrip_us / 1000.0)
            state = integrate_plant(state, control_v, dt, params)
            times.append((sample_id + 1) * dt)
            states.append(state.copy())

            if plot is not None:
                plot.update(times, states, controls, inference_ms, rtt_ms)

            print(
                f"[{sample_id + 1:03d}/{steps:03d}] "
                f"x={state[0]:+.5f} theta={state[1]:+.5f} "
                f"u={control_v:+.5f} V infer={infer_us} us "
                f"RTT={roundtrip_us:.1f} us"
            )

            if not args.no_realtime:
                next_deadline = wall_start + (sample_id + 1) * dt
                remaining = next_deadline - time.perf_counter()
                if remaining > 0.0:
                    time.sleep(remaining)
                else:
                    missed_deadlines += 1

    print(f"CSV guardado: {csv_path.resolve()}")
    print(f"Deadlines perdidos: {missed_deadlines}")
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
