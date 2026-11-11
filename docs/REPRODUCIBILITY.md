# Estado de reproducibilidad

| Etapa | Evidencia conservada | Estado y elemento faltante |
|---|---|---|
| Preparación radiográfica | Pipeline y generador ST | Dataset completo, licencia y manifiesto de particiones por hash pendientes |
| Entrenamiento Float32/QAT RX | Celdas, historial y resultados | No reejecutado; checkpoint QAT y entorno resuelto pendientes |
| Conversión RX | Celdas PTQ/INT16x8/Mixed; modelos privados parciales | Verificar hashes contra logs; no intercambiar interfaces INT8 y Float32 |
| Referencia MPC | Notebook, params, checkpoint y normalización recuperados | Dataset X/U/X0 y vínculo inequívoco del checkpoint pendientes |
| Cuantización MPC | Celdas, salidas, tablas y reportes | Archivos TFLite originales por variante pendientes |
| Comparación PC | CSV/NPZ, HTML, figuras y scripts | Reanálisis posible; nueva inferencia requiere datos/modelos exactos |
| Compilación N6 | run-3/run-10, fuentes generadas, bibliotecas, logs | Recompilación no ejecutada; rutas SDK y hardware pendientes |
| Validación L476 | Logs y listados | Proyecto completo no conservado aquí |
| HIL mixta | Emisor, ocho CSV/logs, planta y firmware histórico | Reconciliar tratamiento angular y registrar hash por nueva ejecución |
| Memoria/latencia | Informes y tablas | No hay medición homogénea Flash/RAM total ni energía |

La exportación Conda es un registro, no una prueba de que todas las campañas usaron exactamente el mismo entorno. Las semillas y criterios que aparecen en los notebooks son conservados; no se añade una semilla global retrospectiva.

Cada nueva campaña debería conservar SHA-256 del modelo y ELF, versión de herramientas, configuración de reloj/memoria, entradas y parámetros I/O, puerto y baudrate, estado de caché/calefacción si se controla, y límites del intervalo medido. Esta lista es una propuesta de trazabilidad futura, no evidencia de controles ejecutados históricamente.

Los datos representativos y auditorías I/O se describen en las celdas de cada conversión. No existe una auditoría universal de todos los modelos después de exportar. Evite completar escalas/puntos cero ausentes con valores de otra variante. Las cifras INT16x8 compiladas deben interpretarse junto con el formato reportado por ST, no sólo con el tipo solicitado en TensorFlow Lite.

El índice audit/notebook_parameters.csv localiza por número de celda las configuraciones de entrenamiento, calibración y conversión; audit/notebook_imports.csv registra las importaciones. Son índices estáticos, no prueba de ejecución ni extracción de algoritmos. Los valores y comentarios completos permanecen en el notebook.

El banco privado radiografias_test_st.npz se abrió con allow_pickle=False: x_test tiene forma (624,150,150,1) y dtype float32; y_test, (624,) int64. El contenedor CNN_Float32.keras registra Keras 3.12.4 y cuatro convoluciones de 6 filtros, seguidas por Dense 16 y Dense 2. Se inspeccionó metadata/configuración, sin ejecutar inferencia. normalization.json conserva mean/scale y orden [x,theta,v,dtheta]; debe contrastarse con la variante y el firmware usados.
