# MPC neuronal y CartPole

Candidato principal: `notebooks/Notebook_MPC_CartPole_Baseline_Original_Autores_resultados.ipynb`; la copia `(1)` comparte el contenido de celdas. Los notebooks NN_Float32, ReplayMPC y otras variantes son históricos y no reemplazan automáticamente la referencia final. Generador_Dataset_Validacion prepara entradas para ST.

Desde la raíz, con Jupyter instalado:

```bash
python -m jupyterlab cases/mpc/notebooks
```

Para ejecutar el baseline, sitúe en el directorio de trabajo del notebook `FinalAMPCPendulumDataset/X.csv`, `U.csv`, `X0.csv`, y `model/model.eqx`, `params.yaml`, `normalization_reconstructed.pkl`. Los tres primeros no están incluidos. Los artefactos recuperados de model están en el material privado; su equivalencia con el checkpoint histórico está pendiente. No cargue pickle de otra procedencia. El repositorio de Hose está enlazado en la bibliografía; falta fijar el commit y verificar el dataset/checkpoint exactos antes de usarlos como reemplazo.

Se transfiere a Keras la política nominal preentrenada, con siete capas ocultas de 50 neuronas tanh y salida escalar. No se vuelve a entrenar Float32 desde cero ni se reproduce el mecanismo adaptativo de sensibilidades del artículo. params.yaml registra num_layers=8 y train_aug=false; params_model_2.yaml es una configuración distinta (num_layers=10, train_aug=true). Las anotaciones históricas sobre cinco capas no sustituyen la topología cargada.

El orden interno es [y, alpha, ydot, alphadot]. El estado físico se normaliza con los parámetros del checkpoint; la salida se desnormaliza y satura a ±9 V. La planta se integra en computador con periodo 0,16 s durante 16 s. Los 200 estados para validación de inferencia ST no son los 200 experimentos de condiciones iniciales del lazo cerrado.

Hay resultados PC de Float32, PTQ por tensor, PTQ por canal, QAT INT8, INT16x8 y precisión mixta. La mixta alcanza 78% frente a 99,5% de Float32 en el banco de simulación; no se presenta como superior funcionalmente. En placa existen varias rutas de inferencia; HIL sólo se conserva para mixta. Véase [HIL y firmware](../../docs/FIRMWARE_UART.md), especialmente la incompatibilidad angular pendiente del proyecto archivado.
