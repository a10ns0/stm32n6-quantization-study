# Integración de radiografías por UART con STM32N6 y X-CUBE-AI

## Archivos

- `app_x-cube-ai.c`: reemplaza el archivo homónimo generado por X-CUBE-AI,
  normalmente ubicado en `Appli/AI/App/app_x-cube-ai.c`.
- `send_xray_uart.py`: se ejecuta en el PC; abre las radiografías, reproduce el
  preprocesamiento del notebook, cuantiza la entrada y la envía al STM32.
- `main.c`: no requiere cambios. Ya declara `huart1`, inicializa USART1 y llama
  `STM32CubeAI_Studio_AI_Init()` / `STM32CubeAI_Studio_AI_Process()`.

No se deben modificar `network.c`, `network_weights.c`, `stai_network.c`,
`network_atonbuf.xSPI2.c`, `npu_init.c`, `npu_cache.c` ni `user_init.c`.

## Configuración de STM32CubeIDE

1. Conserva USART1 en modo asíncrono: 115200 baud, 8 bits, sin paridad, un bit
   de parada y sin control de flujo.
2. Reemplaza `app_x-cube-ai.c` y compila con **Project > Clean**, seguido de
   **Build Project**.
3. Programa la placa y conecta el puerto COM del ST-LINK Virtual COM Port.
4. No abras simultáneamente el mismo COM en PuTTY, Tera Term, el monitor serie
   de CubeIDE u otra aplicación.

Al arrancar, la placa transmite:

```text
XRAY_CLASSIFIER,INT8,150x150x1
READY
```

## Preparación del PC

En la misma instalación de Python/Jupyter usada para el proyecto:

```powershell
pip install numpy pyserial opencv-python scikit-image
```

Enviar las ocho imágenes de validación:

```powershell
python send_xray_uart.py "C:\Users\Alonso\Jupyter\Tesis\Test_Modelos_Cuantizacion\Red_clasificacion_Imagenes\Red_clasificacion_Radiografias\chest_xray\val" --port COM4 --baud 115200
```

Probar primero una sola imagen:

```powershell
python send_xray_uart.py "C:\ruta\imagen.jpeg" --port COM4
```

Limitar una carpeta a dos imágenes:

```powershell
python send_xray_uart.py "C:\ruta\carpeta" --port COM4 --limit 2
```

## Formato de la respuesta

Ejemplo:

```text
RESULT,id=1,class=1,label=PNEUMONIA,normal_milli=43,pneumonia_milli=957,latency_us=18234
```

- Clase `0`: `NORMAL`.
- Clase `1`: `PNEUMONIA`.
- `normal_milli` y `pneumonia_milli` son scores entre 0 y aproximadamente
  996, expresados sobre 1000.
- Debido a que el modelo termina con dos activaciones sigmoid, los scores no
  tienen por qué sumar exactamente 1000.

## Protocolo UART

Cada envío contiene un encabezado de 12 bytes y 22500 bytes de datos:

| Campo | Tamaño | Formato |
|---|---:|---|
| Magic | 4 bytes | ASCII `XRAY` |
| ID | 4 bytes | `uint32`, little-endian |
| Longitud | 4 bytes | `uint32`, little-endian; valor 22500 |
| Tensor | 22500 bytes | INT8, forma 150×150×1 |

A 115200 baud, transferir cada tensor tarda alrededor de dos segundos, además
del tiempo de inferencia. Si posteriormente se cambia USART1 a 921600 baud en
CubeMX, se debe usar también `--baud 921600` en Python.

## Importante al regenerar X-CUBE-AI

Regenerar el código puede reemplazar partes de `app_x-cube-ai.c`. Conserva este
archivo modificado o vuelve a aplicar la integración después de regenerar el
modelo.
