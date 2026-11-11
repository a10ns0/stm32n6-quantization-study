# Entorno

La exportación configs/environment-recorded.yml identifica Python 3.10.20, TensorFlow 2.21.0, Keras 3.12.4, tf-keras 2.21.0, TensorFlow Model Optimization 0.8.1, NumPy 2.2.6, JAX/jaxlib 0.6.2, Equinox 0.13.8 y pyserial 3.5. Es evidencia de un entorno registrado, no un lock probado en esta máquina ni prueba de versiones idénticas en todas las campañas. No se garantiza que todas estas versiones estén disponibles o sean compatibles en otra plataforma.

Los requirements separados se derivan de importaciones y del requirements de análisis ya existente. No fijan versiones inventadas. requirements-notebooks.txt es una lista de dependencias a resolver y comprobar, no una instalación validada. Para QAT se distingue tf-keras de Keras 3; respete las celdas que configuran la interfaz y reinicie el kernel cuando el notebook lo requiera. No mezcle una sesión parcialmente ejecutada de ambos entornos.

Preparación propuesta, todavía no instalada de extremo a extremo, desde la raíz y con Conda disponible:

```bash
conda create -n tesis-edgeai python=3.10
conda activate tesis-edgeai
python -m pip install -r requirements-notebooks.txt
```

La cadena registrada en la memoria es ST Edge AI Core 4.0.1, CubeMX 6.18.1, CubeIDE 2.2.0 y CubeProgrammer 2.23.0. Los logs por campaña prevalecen sobre esta descripción general; aparecen también runtime 12.0.1, compilación de biblioteca GCC 13.3.1 y herramientas GNU 14.3.1. No se atribuye una versión no registrada a Studio ni a Anaconda Navigator. Los proyectos conservan su configuración y los run-settings originales, incluyendo opciones de memoria y compilación.

No se incluye instalación automatizada de herramientas propietarias. Obtenga las versiones compatibles de ST y revise las rutas de herramientas y bibliotecas en .cproject/.launch antes de construir. No se ejecutó build STM32 ni se comprobó acceso a ST-LINK en este entorno.
