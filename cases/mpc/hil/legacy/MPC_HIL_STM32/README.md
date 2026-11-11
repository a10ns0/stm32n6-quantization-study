# HIL AMPC CartPole - STM32N6570-DK

Este paquete reemplaza el firmware de validación de ST Edge AI por un lazo
Hardware-in-the-Loop. El PC integra la planta y la STM32 ejecuta el modelo
`ampc_cartpole_mixedprecision`.

## Datos confirmados del modelo

- Entrada: `Float32 [1,4]`.
- Orden: `[x, theta, v, dtheta]`.
- Salida: `Float32 [1,1]` normalizada.
- Control físico: tensión limitada a `[-9, 9] V`.
- Periodo del simulador original: `Ts = 0.160 s`.
- Integración original: `scipy.integrate.odeint` con entrada constante.
- UART1 VCP: PE5 TX, PE6 RX, 115200-8-N-1.

La normalización se hace dentro de `mpc_ai.c`. El PC siempre transmite estados
físicos sin normalizar.

## 1. Copiar archivos al proyecto

Copiar:

- `Core/Inc/mpc_ai.h` a `Appli/Core/Inc/`.
- `Core/Inc/mpc_hil.h` a `Appli/Core/Inc/`.
- `Core/Src/mpc_ai.c` a `Appli/Core/Src/`.
- `Core/Src/mpc_hil.c` a `Appli/Core/Src/`.

El `Core/Src/main.c` incluido ya contiene los cambios. Se puede usar como guía
o reemplazar el `main.c` del proyecto, conservando antes una copia.

Los únicos cambios funcionales de `main.c` son:

```c
#include "mpc_ai.h"
#include "mpc_hil.h"
```

Después de inicializar USART1:

```c
if (!MPC_AI_Init()) {
    Error_Handler();
}
if (!MPC_HIL_Init()) {
    Error_Handler();
}
```

Y dentro del `while (1)`:

```c
MPC_HIL_Process();
```

No llamar a `STM32CubeAI_Studio_AI_Init()` ni a
`STM32CubeAI_Studio_AI_Process()`, porque activan el protocolo de validación de
ST y competirían por USART1.

## 2. Compilar y programar

1. Hacer `Project > Clean`.
2. Hacer `Build Project` para Appli y FSBL según el flujo actual de la placa.
3. Programar los binarios Trusted necesarios como en las pruebas anteriores.
4. Ejecutar la aplicación en la placa.

Al iniciar, la placa transmite:

```text
READY,MPC_HIL,V1,STATE_ORDER=x_theta_v_dtheta
```

## 3. Preparar el PC

```bash
python -m pip install -r PC/requirements.txt
```

Prueba local de ecuaciones y formato de paquetes:

```bash
python PC/hil_cartpole_uart.py --self-test
```

Ensayo HIL de 16 segundos desde 0.30 rad:

```bash
python PC/hil_cartpole_uart.py --port COM4 --theta0 0.30 --duration 16
```

El script crea un CSV, muestra los estados y el control en tiempo real y
compara la latencia exclusiva de inferencia con el RTT UART completo.

## Protocolo binario

Todos los valores usan little-endian e IEEE-754 Float32.

Solicitud PC a STM32, 24 bytes:

```text
<4sI4f = "MPC1", id, x, theta, v, dtheta
```

Respuesta STM32 a PC, 28 bytes:

```text
<4sIffIIi = "MPR1", id, u_V, u_normalizada,
             inference_us, inference_cycles, status
```

`status=0` indica éxito. El tiempo `inference_us` mide únicamente
`stai_mnetwork_run()`. El PC mide por separado el RTT completo.

## Observaciones

- Las capas seleccionadas INT8 se ejecutan dentro de una red cuya entrada y
  salida son Float32; no se debe cuantizar el paquete UART.
- El código generado contiene kernels CPU `forward_lite_*`. La presencia de
  `npu_init.c` y del middleware Neural-ART no demuestra que estas capas estén
  ejecutándose en la NPU.
- El ángulo se envuelve a `[-pi, pi)` antes de normalizar, igual que en el
  simulador de referencia.
- Para comparar cuantizaciones, conservar la misma condición inicial, periodo,
  ecuaciones y versión del script PC.
