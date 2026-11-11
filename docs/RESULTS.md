# Resultados y trazabilidad

La evidencia íntegra se encuentra en [originales](../analysis/evidencia/resultados/originales), organizada por CasoRadiografia, CasoMPC y NUCLEO-L476RG. Los inventarios conservan hashes y las tablas CSV enlazan el nombre de archivo y, cuando existe, el hash de modelo de ST. [Inventario de fuentes](../analysis/tablas/resultados/inventario_fuentes.csv).

| Pregunta | Tabla derivada | Fuente y límite |
|---|---|---|
| Clasificación en computador | [Comparación RX](../analysis/tablas/resultados/radiografia_comparacion.csv) | Predicciones NPZ/CSV de 624 imágenes; no prueba clínica externa |
| Cambio de decisiones | [Comparación pareada](../analysis/tablas/resultados/radiografia_predicciones_pareadas.csv) | Mismo banco; concordancia con Float32 no equivale a exactitud clínica |
| Lazo cerrado MPC | [Simulación](../analysis/tablas/resultados/mpc_simulacion_resumen.csv) | Banco de 200 condiciones; no confundir con validación de inferencia ST |
| Inferencia N6 | [Registros ST](../analysis/tablas/resultados/validacion_st.csv) | Tiempo y memoria del resumen de generación diferenciados del resumen de validación |
| Comparación de placas | [Tabla](../analysis/tablas/resultados/comparacion_placas.csv) | Emparejamiento por modelo; hardware, frecuencia y bibliotecas distintos |
| Memoria por variante/ruta | [Ambas placas](../analysis/tablas/resultados/validacion_ambas_placas.csv) | Pesos y activaciones, no Flash/RAM total |
| HIL | [Resumen](../analysis/tablas/resultados/hil_resumen.csv) | Inferencia, RTT y retrasos PC separados; estado final reconstruido identificado |
| Helium y DSP | [Auditoría de 23 listados](../analysis/tablas/resultados/auditoria_23_listados.csv) | Presencia estática; no prueba de ejecución de cada función |

Las figuras están en [figuras/resultados](../analysis/figuras/resultados). La explicación extensa y las leyendas permanecen en [Resultados](../analysis/capitulos/09_resultados.tex) y [Discusión](../analysis/capitulos/10_seleccion_discusion.tex). El eje de imágenes usa enteros; las curvas angulares MPC y gráficos de memoria ya forman parte de esta copia.

En radiografías, INT8/NPU responde a la prioridad temporal; Mixed-5/M55 conserva las decisiones de referencia del banco estudiado, con mayor coste temporal. En MPC, la mixta es una alternativa parcialmente cuantizada, no una mejora global sobre Float32. La presencia de Helium contribuye a describir la implementación CPU, pero la comparación L476/N6 no permite aislar su beneficio de los demás cambios de plataforma.

No se transforman estimaciones en medidas: no hay consumo energético experimental ni caracterización de Flash/RAM total para cada variante. Un error cruzado ST compara implementaciones del modelo; no mide directamente error respecto al optimizador MPC ni estabilidad del sistema.
