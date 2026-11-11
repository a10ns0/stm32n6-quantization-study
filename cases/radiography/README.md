# Radiografías

Los notebooks principales candidatos son `notebooks/Pipeline_Radiografias (1).ipynb` y `notebooks/comparacion_modelos_resumen.ipynb`; generacion_data_xray_st_studio prepara bancos de entrada. Las otras variantes se preservan como historia. No se ejecutan concatenadamente.

Desde la raíz del repositorio, con Jupyter instalado:

```bash
python -m jupyterlab cases/radiography/notebooks
```

Abra el pipeline y ejecute desde su directorio después de preparar chest_xray/train y chest_xray/test. Las rutas de modelos y archivos de validación se deben revisar en las celdas antes de ejecutar; no hay un CLI equivalente. El dataset completo no se incluye. La procedencia y licencia deben verificarse contra la referencia del dataset en la memoria y la copia exacta utilizada, sin sustituirla por otro reparto con el mismo nombre.

El pipeline aplica imágenes en gris, resize a 150×150×1 y escala [0,1]. Conserva 2682 ejemplos de entrenamiento balanceados y 624 de evaluación (234 NORMAL y 390 PNEUMONIA). El conjunto de 624 interviene en validación durante el entrenamiento: no debe describirse como una prueba externa independiente. La referencia usa cinco épocas, Adam y sparse_categorical_crossentropy; la salida de dos sigmoides no exige scores que sumen uno. QAT registra ocho épocas y una partición 2279/403; el procedimiento documenta seed 42 para partición y calibración de 350 imágenes. No se acredita determinismo global de todos los entrenamientos.

Se evaluaron Float32, PTQ INT8, QAT INT8, INT16x8 y Mixed-3/Mixed-5 en computador. La comparación PTQ incluye una arquitectura de 8 canales y 30682 parámetros frente a 6 canales y 22700 en la referencia: la diferencia no se atribuye exclusivamente a cuantización. El despliegue conservado run-3 corresponde a Mixed-5 y validación ST, no a todos los modelos ni al protocolo UART personalizado.

El directorio uart conserva el emisor y la integración C alternativa. Su guía histórica contiene rutas locales; úsela junto con [la documentación de protocolos](../../docs/FIRMWARE_UART.md). Los registros de 8 entradas del validador ST y de 16 imágenes UART son bancos distintos. No se ha validado uso clínico.
