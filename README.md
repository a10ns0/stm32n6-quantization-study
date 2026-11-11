# Cuantización de redes neuronales en STM32N6

Material experimental de la memoria de Ingeniería Civil Electrónica de Alonso Castillo Pinto. Se estudia el compromiso entre fidelidad funcional, latencia de inferencia y recursos de memoria al desplegar redes cuantizadas en STM32N6570-DK. Los casos son clasificación NORMAL/PNEUMONIA y aproximación neuronal de MPC para CartPole. NUCLEO-L476RG constituye una comparación complementaria.

**Estado: reproducción parcial.** Se pueden volver a analizar los registros y examinar los notebooks. No se certifica la repetición integral del entrenamiento ni de todas las compilaciones. El firmware MPC archivado todavía reduce el ángulo antes de normalizar: no identifica inequívocamente la versión de los ocho ensayos HIL actualizados. Véase [auditoría](docs/AUDIT.md).

## Technical summary

This repository collects experimental evidence for neural-network quantization on STM32N6570-DK using chest X-ray classification and neural MPC approximation. It separates host evaluation, compiled-model validation and closed-loop HIL. INT8, INT16x8 and selective mixed precision are compared where evidence exists. Reproducibility is partial; hardware execution, numerical fidelity and memory metrics must be interpreted separately. No clinical validation or hardware-independent speedup claim is made.

## Contribución y alcance

Se compara cómo una elección de precisión afecta tanto la tarea como la implementación real. En MPC, un error estático pequeño no garantiza estabilización en lazo cerrado; en radiografías, concordancia con Float32 no equivale a validación clínica. La precisión mixta selectiva y INT16x8 exploran alternativas cuando INT8 degrada el comportamiento, sin suponer que ambas puedan ejecutarse de forma nativa en NPU. El controlador nominal preentrenado procede del trabajo de Hose y colaboradores: su arquitectura y entrenamiento original no son contribuciones propias.

El flujo es preparación de datos → referencia Float32 → cuantización/ajuste QAT → evaluación en computador → análisis y compilación ST → validación en placa → UART/HIL para las alternativas integradas.

| Ruta | Contenido |
|---|---|
| [cases/radiography](cases/radiography/README.md) | Notebooks originales y aplicación UART |
| [cases/mpc](cases/mpc/README.md) | Notebooks originales y emisor HIL, con historia separada |
| [firmware](docs/FIRMWARE_UART.md) | Proyectos run-3 y run-10; fuentes, bibliotecas y configuración STM32 |
| [analysis](analysis) | Memoria LaTeX, evidencia original, tablas, figuras y scripts de análisis con rutas conservadas |
| [configs](configs) | Exportación histórica de entorno y parámetros MPC |
| [audit](audit) | Inventarios, hashes, exclusiones y controles |
| [references](references/PAPERS.md) | Bibliografía y relación entre literatura e implementación |

## Entorno y ejecución

Python y Jupyter son necesarios para los notebooks; las dependencias se detallan en [entorno](docs/ENVIRONMENT.md). STM32CubeIDE, la cadena ST Edge AI y una placa física son necesarios para compilar y validar. No hay hardware disponible en la verificación de este paquete.

Desde la raíz del repositorio, en un entorno Python aislado:

```bash
python -m pip install -r requirements-analysis.txt
python tools/check_repository.py
python analysis/scripts_resultados/analizar.py
python analysis/scripts_resultados/graficos.py
python analysis/scripts_resultados/tablas.py
python analysis/scripts_resultados/auditar_instrucciones.py
python analysis/scripts_resultados/ampliacion_placas.py
python analysis/scripts_resultados/auditar_nucleo.py
python analysis/scripts_resultados/preparar_figuras_detalle.py
```

Estos scripts reconstruyen tablas y figuras sobre registros existentes; no realizan inferencias nuevas. Sobrescriben resultados derivados. Conserve un commit antes de ejecutarlos. Los grafos originales y sus PDF de presentación están incluidos; su generación desde el compilador exige los modelos y herramientas correspondientes.

Para verificar el emisor sin placa, desde la raíz:

```bash
python -m pip install -r requirements-hil.txt
python cases/mpc/hil/hil_cartpole_uart.py --self-test
```

La ejecución de notebooks, la preparación de datos y el HIL se describen en las páginas de cada caso. No se ha extraído ni refactorizado el algoritmo de los notebooks. No ejecute todos los históricos como si fueran una secuencia.

## Resultados principales

| Comparación | Resultado registrado | Evidencia |
|---|---|---|
| MPC, simulación de 200 condiciones iniciales | Float32 99,5%; INT16x8 99%; mixta 78%; PTQ tensor 37%; PTQ canal 25%; QAT 32,5% | [Resumen](analysis/tablas/resultados/mpc_simulacion_resumen.csv) |
| MPC, validación de inferencia N6 | Mixta M55 0,326 ms; mixta ruta NPU 0,480 ms | [Validación](analysis/tablas/resultados/validacion_st.csv) |
| Radiografías, QAT, inferencia | L476 289,351 ms; M55 14,814 ms; ruta NPU 0,290 ms | [Comparación](analysis/tablas/resultados/comparacion_placas.csv) |
| HIL, alternativa mixta | 5 de 8 condiciones cumplen el criterio final documentado | [Resumen HIL](analysis/tablas/resultados/hil_resumen.csv) |

La comparación entre placas mezcla frecuencia, arquitectura, bibliotecas y memoria; no mide un efecto causal aislado de Helium. Los listados son evidencia estática de instrucciones presentes. El formato INT16x8 exportado no acredita ejecución nativa INT16x8. Pesos y activaciones no representan la Flash/RAM total del firmware. No se midió consumo energético.

Para detalles, figuras y límites: [resultados y trazabilidad](docs/RESULTS.md), [reproducibilidad](docs/REPRODUCIBILITY.md), [comprobaciones](audit/CHECKS.md).

## Citación, licencias y publicación

[CITATION.cff](CITATION.cff) contiene autor y título descriptivo del repositorio; faltan DOI, versión pública e identificación bibliográfica definitiva de la memoria. [Bibliografía principal](references/principal.bib) y [correspondencia con papers](references/PAPERS.md). La licencia del código propio queda pendiente: [licencias](LICENSE_STATUS.md). Se mantienen las licencias de ST y Arm en los proyectos. No se incluyen los PDF científicos ni se autoriza por extensión la redistribución de datasets o checkpoints externos.

Nombre sugerido: `stm32n6-quantization-study`. Descripción: «Evaluación reproducible de cuantización de redes neuronales para radiografías y MPC en STM32N6, con validación embebida y HIL».

[Instrucciones de publicación](docs/PUBLISH.md). Repositorio público: https://github.com/a10ns0/stm32n6-quantization-study.
