# Comprobaciones y cambios

## Ejecutado

- Inventario de entradas con SHA-256; los cuatro ZIP Resultados son idénticos.
- 19 notebooks copiados byte a byte y validados como JSON nbformat 4; sin ejecución de sus celdas.
- Sintaxis Python por AST, tamaños y empaquetado básico UART 24/28 bytes.
- Self-test del emisor HIL: correcto, sin puerto serie ni placa.
- Reanálisis de registros con analizar.py: correcto; verifica predicciones pareadas, reconstrucción de estados HIL y consistencia de resultados.
- Inspección segura de claves, formas y tipos del NPZ radiográfico; metadata y arquitectura del contenedor Keras. No se cargaron objetos pickle ni se ejecutó el modelo.
- Escaneo de patrones concretos de credenciales: ningún candidato detectado. No equivale a certificación exhaustiva de ausencia de secretos. Las rutas absolutas se enumeran por archivo y línea.

## No ejecutado o bloqueado

- `send_xray_uart.py --help` no alcanzó argparse: falta cv2 en este entorno. No se modificó el emisor para ocultar la dependencia.
- No están instalados TensorFlow ni los bindings TFLite; no se comprobó carga/inferencia de esos modelos.
- Sin compilación CubeIDE, programación de placa, UART real, entrenamiento ni nuevos ensayos HIL.
- No se instaló ni resolvió el entorno histórico completo; requirements no es un lock validado.
- No se compilaron los capítulos LaTeX en esta comprobación del repositorio.
- No se publicó en GitHub ni se otorgó licencia al código propio.

## Cambios

Se creó una copia independiente, documentación nueva, inventarios, bibliografía principal, CITATION.cff, requirements por tarea y .gitignore. Se preservaron notebooks, fuentes de firmware y evidencia; se omitieron productos de build enumerados. Los resultados derivados fueron regenerados. No se refactorizó el algoritmo de investigación ni se corrigió silenciosamente la convención angular histórica. No se añadió CI que simule verificar hardware.

## Regeneración comprobada

Los seis scripts graficos.py, tablas.py, auditar_instrucciones.py, ampliacion_placas.py, auditar_nucleo.py y preparar_figuras_detalle.py terminaron con código 0. script_checks.json registra cada resultado. Los enlaces Markdown locales de la documentación nueva fueron comprobados. La cobertura de los ZIP de resultados se verificó por hash para 138 miembros de archivo (result_archive_coverage.csv).
