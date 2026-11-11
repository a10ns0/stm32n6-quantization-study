# Auditoría y decisiones de organización

## Verificado

Los cuatro ZIP Resultados, Resultados(1), Resultados(2) y Resultados(3) son idénticos por SHA-256. La copia expandida conserva sus resultados y las incorporaciones QAT/L476 de la memoria más reciente. Se conservan 19 notebooks sin modificar sus celdas ni salidas. El inventario registra hashes del archivo completo y del contenido de las celdas: igualdad de código no prueba igualdad de salidas. Pipeline_Radiografias (1), (1) y (2) comparten contenido de celdas; también Baseline_Original_Autores (1) y _resultados. No se descartó ningún notebook por su nombre.

La memoria utilizada es la versión editorial Memoria_Estilo_Academico. Los borradores editoriales anteriores no se incorporan como capítulos concurrentes. Las fuentes experimentales mantienen su jerarquía bajo analysis para que los scripts existentes sigan resolviendo sus entradas. Los objetos y carpetas Debug/Release de los ZIP STM32 se excluyen de la copia organizada; el inventario de exclusiones conserva sus hashes. Las bibliotecas .a, los fuentes de red generados, los linker scripts, FSBL, .ioc y proyectos de IDE se mantienen. Los .list usados como evidencia se conservan bajo analysis, aunque no sean necesarios para recompilar.

## Inferencias delimitadas

La selección de notebooks principales se apoya en la presencia de conversiones, tablas y secuencias utilizadas en la memoria. No existe un registro de ejecución que vincule cada notebook por SHA-256 con cada modelo y cada log. Por ello se denominan candidatos principales, no versiones finales certificadas. Los modelos privados recuperados no se asignan a una campaña únicamente por el nombre.

## Discrepancia angular confirmada

En firmware/mpc/run-10/Appli/Core/Src/mpc_ai.c, MPC_AI_Run construye state_for_model con wrap_to_pi(state_phys[1]). La copia bajo STM32CubeIDE/Appli/Application/User/Core presenta la misma operación. El emisor HIL actualizado y el procedimiento de la memoria conservan el ángulo continuo antes de normalizar. Se conserva el firmware histórico sin modificarlo. Falta recuperar el proyecto exacto de los ensayos actualizados o aplicar y validar explícitamente el cambio en una nueva campaña. Los CSV existentes no incluyen hash del firmware: no permiten cerrar por sí solos esa asociación. Esto no demuestra que los ensayos sean incorrectos; impide certificar su reproducción exacta con run-10.

## Portabilidad y privacidad

Los notebooks, logs, archivos .launch, .stai y configuraciones contienen rutas Windows y Linux de entornos originales. audit/path_findings.csv identifica archivos y líneas; las rutas no se presentan como instalación portable. Los logs y notebooks se conservan como evidencia sin alterar valores. La exportación Conda incluye builds Windows y prefix local: no debe instalarse indiscriminadamente en Linux. No se han utilizado credenciales para publicar. El escaneo automático de patrones no sustituye la revisión previa a una publicación pública.

Los originales descargados y los ZIP de firmware permanecen separados de esta copia de trabajo. El directorio hermano material_privado_no_publicar contiene modelos y datos recuperados cuya licencia/procedencia exacta todavía debe confirmarse; no forma parte del repositorio ni debe añadirse con git add desde su carpeta padre.

## Pendiente

Recuperar los datasets completos X.csv/U.csv/X0.csv y chest_xray, los modelos MPC TFLite por variante, el modelo radiográfico QAT y los bancos exactos usados en cada validación. Se recuperaron model.eqx, normalización y parámetros, pero falta verificar su correspondencia con el checkpoint de cada experimento. La versión aumentada params_model_2.yaml no se intercambia con params.yaml: train_aug y profundidad difieren. No hay firmware L476 completo en el ZIP de resultados de esa placa.

Inventarios: [entradas](../audit/input_inventory.csv), [notebooks](../audit/notebooks.csv), [productos excluidos](../audit/build_products_excluded.csv). Las versiones y fuentes no disponibles se declaran como pendientes, no se reconstruyen por suposición.

## Diferencia entre instantáneas de resultados

El HTML Pipeline_Radiografias de la memoria no coincide byte a byte con el del ZIP Resultados. Se conservan ambos: el original del ZIP en analysis/evidencia/archive_original y la instantánea usada por el análisis en su ruta histórica. No se sustituyó una por otra ni se certifica equivalencia total. Todos los miembros de los ZIP Resultados y NUCLEO están representados por contenido en la evidencia.
