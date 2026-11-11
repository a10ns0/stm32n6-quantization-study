#!/usr/bin/env python3
"""Envía radiografías al STM32N6 y muestra la clasificación recibida por UART.

El preprocesamiento replica el usado en Pipeline_Radiografias:
OpenCV -> resize de skimage -> rgb2gray -> cuantización INT8.
"""

from __future__ import annotations

import argparse
import struct
import sys
import time
from pathlib import Path

import cv2
import numpy as np
import serial
from skimage.color import rgb2gray
from skimage.transform import resize


MAGIC = b"XRAY"
WIDTH = 150
HEIGHT = 150
INPUT_BYTES = WIDTH * HEIGHT
INPUT_SCALE = np.float32(0.00392156885936856)  # 1/255
INPUT_ZERO_POINT = -128
IMAGE_EXTENSIONS = {".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff"}


def preprocess_image(path: Path) -> np.ndarray:
    """Devuelve el tensor plano INT8 esperado por CNN_PTQ_MIXED_SELECTIVE."""
    image = cv2.imread(str(path), cv2.IMREAD_COLOR)
    if image is None:
        raise ValueError(f"No se pudo leer la imagen: {path}")

    # Se conserva deliberadamente el mismo flujo del notebook de entrenamiento.
    image = resize(
        image,
        (HEIGHT, WIDTH, 3),
        mode="constant",
        anti_aliasing=True,
    )
    image = rgb2gray(image).astype(np.float32)

    quantized = np.rint(image / INPUT_SCALE + INPUT_ZERO_POINT)
    quantized = np.clip(quantized, -128, 127).astype(np.int8)

    tensor = np.ascontiguousarray(quantized.reshape(HEIGHT, WIDTH, 1))
    if tensor.nbytes != INPUT_BYTES:
        raise RuntimeError(f"Tensor incorrecto: {tensor.shape}, {tensor.nbytes} bytes")
    return tensor


def find_images(path: Path) -> list[Path]:
    if path.is_file():
        if path.suffix.lower() not in IMAGE_EXTENSIONS:
            raise ValueError(f"Extensión de imagen no soportada: {path.suffix}")
        return [path]

    if not path.is_dir():
        raise FileNotFoundError(path)

    return sorted(
        item
        for item in path.rglob("*")
        if item.is_file() and item.suffix.lower() in IMAGE_EXTENSIONS
    )


def read_result(ser: serial.Serial, timeout_s: float) -> str:
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        raw = ser.readline()
        if not raw:
            continue
        line = raw.decode("utf-8", errors="replace").strip()
        if line:
            print(f"STM32> {line}")
        if line.startswith("RESULT,") or line.startswith("ERR,"):
            return line
    raise TimeoutError("El STM32 no devolvió RESULT/ERR dentro del tiempo esperado")


def wait_for_ack(
    ser: serial.Serial,
    expected: str,
    timeout_s: float,
) -> None:
    """Espera un ACK antes de continuar con la siguiente fase del protocolo."""
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        raw = ser.readline()
        if not raw:
            continue
        line = raw.decode("utf-8", errors="replace").strip()
        if line:
            print(f"STM32> {line}")
        if line == expected:
            return
        if line.startswith("ERR,"):
            raise RuntimeError(f"El firmware informó: {line}")
    raise TimeoutError(f"El STM32 no devolvió {expected}")


def send_image(
    ser: serial.Serial,
    image_id: int,
    image_path: Path,
    result_timeout_s: float,
) -> str:
    tensor = preprocess_image(image_path)
    payload = tensor.tobytes(order="C")
    header = struct.pack("<4sII", MAGIC, image_id, len(payload))

    # Primero se envía solamente el encabezado. Esperar el ACK evita perder
    # bytes del payload mientras el STM32 cambia de transmisión a recepción.
    ser.write(header)
    ser.flush()
    wait_for_ack(ser, "ACK,HEADER", timeout_s=5.0)

    ser.write(payload)
    ser.flush()
    return read_result(ser, result_timeout_s)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Clasifica radiografías con un STM32N6 mediante UART"
    )
    parser.add_argument(
        "path",
        type=Path,
        help="Imagen o carpeta (por ejemplo, chest_xray\\val)",
    )
    parser.add_argument("--port", default="COM4", help="Puerto serie (por defecto: COM4)")
    parser.add_argument("--baud", type=int, default=115200, help="Baudios")
    parser.add_argument(
        "--timeout",
        type=float,
        default=120.0,
        help="Tiempo máximo por inferencia, en segundos",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Procesar solamente las primeras N imágenes",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    images = find_images(args.path)
    if args.limit is not None:
        images = images[: args.limit]
    if not images:
        print("No se encontraron imágenes.", file=sys.stderr)
        return 2

    print(f"Imágenes: {len(images)}")
    print(f"Abriendo {args.port} a {args.baud} baud...")

    with serial.Serial(
        port=args.port,
        baudrate=args.baud,
        timeout=0.25,
        write_timeout=30.0,
    ) as ser:
        # Descarta texto parcial que pudiera haber quedado de un arranque anterior.
        time.sleep(0.25)
        ser.reset_input_buffer()

        for image_id, image_path in enumerate(images, start=1):
            print(f"\n[{image_id}/{len(images)}] {image_path}")
            try:
                result = send_image(
                    ser,
                    image_id=image_id,
                    image_path=image_path,
                    result_timeout_s=args.timeout,
                )
            except Exception as exc:
                print(f"ERROR: {exc}", file=sys.stderr)
                return 1

            if result.startswith("ERR,"):
                print("El firmware informó un error; se detiene el envío.", file=sys.stderr)
                return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
