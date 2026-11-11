#ifndef MPC_AI_H
#define MPC_AI_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Orden físico esperado por el modelo: [x, theta, v, dtheta]. */
bool MPC_AI_Init(void);

/*
 * Ejecuta una inferencia.
 * state_phys: estado físico sin normalizar [m, rad, m/s, rad/s].
 * control_v: tensión de control desnormalizada y saturada a [-9, 9] V.
 * output_normalized: salida directa Float32 de la red (puede ser NULL).
 * latency_us/cycles: tiempo exclusivo de stai_mnetwork_run (pueden ser NULL).
 */
bool MPC_AI_Run(const float state_phys[4],
                float *control_v,
                float *output_normalized,
                uint32_t *latency_us,
                uint32_t *cycles);

uint32_t MPC_AI_GetLastError(void);
uint32_t MPC_AI_GetCyclesPerMicrosecond(void);

#ifdef __cplusplus
}
#endif

#endif /* MPC_AI_H */
