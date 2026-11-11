#include "mpc_hil.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "main.h"
#include "mpc_ai.h"
#include "usart.h"

#define MPC_REQUEST_MAGIC  "MPC1"
#define MPC_RESPONSE_MAGIC "MPR1"
#define MPC_UART_TIMEOUT_MS 1000U

typedef struct __attribute__((packed)) {
    char magic[4];
    uint32_t id;
    float state[4]; /* [x, theta, v, dtheta] */
} mpc_request_t;

typedef struct __attribute__((packed)) {
    char magic[4];
    uint32_t id;
    float control_v;
    float output_normalized;
    uint32_t inference_us;
    uint32_t inference_cycles;
    int32_t status;
} mpc_response_t;

_Static_assert(sizeof(mpc_request_t) == 24U, "Unexpected MPC request size");
_Static_assert(sizeof(mpc_response_t) == 28U, "Unexpected MPC response size");

static HAL_StatusTypeDef receive_magic(void)
{
    uint8_t window[4] = {0U, 0U, 0U, 0U};
    uint32_t count = 0U;

    for (;;) {
        uint8_t byte = 0U;
        HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, &byte, 1U, HAL_MAX_DELAY);
        if (status != HAL_OK) {
            return status;
        }

        if (count < 4U) {
            window[count++] = byte;
        } else {
            window[0] = window[1];
            window[1] = window[2];
            window[2] = window[3];
            window[3] = byte;
        }

        if ((count == 4U) && (memcmp(window, MPC_REQUEST_MAGIC, 4U) == 0)) {
            return HAL_OK;
        }
    }
}

static HAL_StatusTypeDef receive_request(mpc_request_t *request)
{
    HAL_StatusTypeDef status = receive_magic();
    if (status != HAL_OK) {
        return status;
    }

    memcpy(request->magic, MPC_REQUEST_MAGIC, 4U);
    return HAL_UART_Receive(&huart1,
                            ((uint8_t *)request) + 4U,
                            (uint16_t)(sizeof(*request) - 4U),
                            MPC_UART_TIMEOUT_MS);
}

bool MPC_HIL_Init(void)
{
    static const uint8_t ready_message[] =
        "READY,MPC_HIL,V1,STATE_ORDER=x_theta_v_dtheta\r\n";

    return HAL_UART_Transmit(&huart1,
                             (uint8_t *)ready_message,
                             (uint16_t)(sizeof(ready_message) - 1U),
                             MPC_UART_TIMEOUT_MS) == HAL_OK;
}

void MPC_HIL_Process(void)
{
    mpc_request_t request;
    float state[4];
    float control_v = 0.0f;
    float output_normalized = 0.0f;
    uint32_t inference_us = 0U;
    uint32_t inference_cycles = 0U;
    mpc_response_t response = {
        .magic = MPC_RESPONSE_MAGIC,
        .id = 0U,
        .control_v = 0.0f,
        .output_normalized = 0.0f,
        .inference_us = 0U,
        .inference_cycles = 0U,
        .status = 0
    };

    if (receive_request(&request) != HAL_OK) {
        return;
    }

    response.id = request.id;
    memcpy(state, request.state, sizeof(state));

    for (uint32_t i = 0U; i < 4U; ++i) {
        if (!isfinite(state[i])) {
            response.status = -1;
            (void)HAL_UART_Transmit(&huart1,
                                    (uint8_t *)&response,
                                    (uint16_t)sizeof(response),
                                    MPC_UART_TIMEOUT_MS);
            return;
        }
    }

    if (!MPC_AI_Run(state,
                    &control_v,
                    &output_normalized,
                    &inference_us,
                    &inference_cycles)) {
        response.status = -(int32_t)MPC_AI_GetLastError();
    }

    response.control_v = control_v;
    response.output_normalized = output_normalized;
    response.inference_us = inference_us;
    response.inference_cycles = inference_cycles;

    (void)HAL_UART_Transmit(&huart1,
                            (uint8_t *)&response,
                            (uint16_t)sizeof(response),
                            MPC_UART_TIMEOUT_MS);
}
