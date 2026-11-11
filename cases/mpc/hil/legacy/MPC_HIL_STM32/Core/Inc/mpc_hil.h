#ifndef MPC_HIL_H
#define MPC_HIL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool MPC_HIL_Init(void);
void MPC_HIL_Process(void);

#ifdef __cplusplus
}
#endif

#endif /* MPC_HIL_H */
