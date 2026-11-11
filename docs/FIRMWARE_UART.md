# Firmware, validación y UART

## Preparación STM32

Los proyectos de firmware/radiography/run-3 y firmware/mpc/run-10 incluyen FSBL, Appli, configuración CubeMX, scripts de enlace y proyectos CubeIDE. Importe los proyectos existentes desde STM32CubeIDE mediante Import → Existing Projects into Workspace. Revise .launch, .cproject y run-settings.json: conservan rutas locales y no constituyen una instalación portable sin ajustes. Construya FSBL y Appli con los paquetes ST correspondientes. La secuencia concreta de carga, los switches de arranque y la configuración de depuración deben verificarse con la revisión de placa y el .launch; no se inventa una posición de switches. Un build exitoso de FSBL por sí solo no demuestra que se esté ejecutando Appli ni el receptor UART.

Los modelos, bancos de validación y opciones de compilación figuran en run-settings.json; sus rutas absolutas se deben relocalizar. run-3 selecciona Mixed-5, ST Neural-ART y opciones native-float/cache/epoch-controller. run-10 identifica AMPC_CartPole_MixedPrecision con optimización balanced y API st-ai. No se infiere asignación a NPU por el nombre de proyecto. Los informes dentro de .ai y los logs por variante son la evidencia de asignación y memoria.

## MPC

USART1, 115200 bit/s, 8N1, sin control de flujo. Sólo un proceso debe abrir el puerto. El mensaje READY identifica el receptor, pero haberlo perdido al conectar no acredita por sí solo un fallo de protocolo.

| Mensaje | Formato little-endian | Bytes | Contenido |
|---|---|---:|---|
| Solicitud | `<4sI4f` | 24 | MPC1, identificador uint32, estado físico [y,alpha,ydot,alphadot] float32 |
| Respuesta | `<4sIffIIi` | 28 | MPR1, id, acción en V, salida normalizada, inferencia en us, ciclos, estado int32 |

El firmware rechaza entradas no finitas. inference_us delimita la ejecución de red; no incluye el ciclo PC ni todo el intercambio UART. RTT se mide en el computador. La normalización y transformación de salida no deben contarse como inferencia si quedan fuera del intervalo DWT.

**Bloqueo de reproducción exacta:** run-10 reduce el ángulo con wrap_to_pi antes de normalizar. El emisor actualizado no lo hace. Recupere el firmware final o valide una corrección identificada por commit antes de comparar una nueva campaña con los ocho CSV históricos. No se ha aplicado silenciosamente esa modificación.

El siguiente comando tiene argumentos verificados en el emisor, pero NO se ha ejecutado contra hardware. Desde la raíz, una vez reconciliado y programado el firmware, con COM4 como puerto realmente asignado al dispositivo:

```bash
python cases/mpc/hil/hil_cartpole_uart.py --port COM4 --alpha0 3.13 --alphadot0 0 --duration 16 --csv hil_alpha3p13.csv --no-plot
```

Para los ocho casos documentados, repita el mismo comando con alpha0 igual a -3.13, -1, -0.3, -0.1, 0.1, 0.3, 1 y 3.13, asignando CSV distintos. Las demás condiciones iniciales por defecto son cero. `--no-plot` altera la carga del PC frente a ensayos con gráfico: documente esa diferencia y no mezcle sus tiempos de ciclo.

Los CSV históricos contienen 100 estados anteriores a la acción, hasta 15,84 s. El análisis reconstruye el estado a 16 s con la planta y la última acción. El criterio HIL verifica |y|<0,4, acción≤9 V y, en los últimos diez estados, |y|<0,1 y error angular reducido<0,17 rad. Este criterio no se intercambia sin revisión con los predicados de éxito de todos los notebooks. El wrap del error para evaluación no es normalización de la entrada neuronal.

## Radiografías

La validación ST usa AI Runner y el middleware aiValidation_ST_AI.c. Es un protocolo distinto de la aplicación personalizada. run-3 conserva validación ST; la integración alternativa está en cases/radiography/uart/app_x-cube-ai.c y requiere reemplazo explícito en el proyecto y recompilación. No se aplicó ese reemplazo a la copia histórica.

La aplicación personalizada envía cabecera XRAY + id uint32 + longitud uint32 (12 bytes) y 22500 bytes INT8; la respuesta es una línea RESULT con clase, scores y latencia. El emisor reproduce el preprocesamiento y la cuantización. Revise escala y punto cero contra el modelo generado antes de usarla con otra variante. Para Mixed-5 se documenta entrada INT8 escala 0,00392156885936856, cero -128; salida escala 1/256, cero -128. No se extrapolan estos valores a otros modelos.

Desde la raíz, la ayuda del emisor no requiere placa:

```bash
python cases/radiography/uart/send_xray_uart.py --help
```

No se incluye un comando de transmisión con una ruta ficticia a imágenes. Consulte el argumento posicional mostrado por --help y use un archivo o directorio real, disponible y autorizado. Una transferencia de imagen a 115200 bit/s tiene un coste muy superior al tiempo de inferencia NPU; no confunda ambas latencias.
