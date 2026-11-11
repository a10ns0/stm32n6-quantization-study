#include "mpc_ai.h"

#include <math.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"
#include "npu_init.h"
#include "network.h"

/*
 * Normalización reconstruida del modelo ampc_cartpole_mixedprecision.
 * La tupla guardada es (media, escala), y se aplica:
 *     x_norm = (x_fisico - media) / escala
 *     u_fisico = u_norm * escala_u + media_u
 */
static const float k_state_mean[4] = {
    0.00006710025472951965f,
    0.004411140706955977f,
    0.00018713739563526303f,
   -0.0037803225950693805f
};

static const float k_state_scale[4] = {
    0.3499989454144998f,
    6.283180170897116f,
    1.199998864994466f,
    9.999998439106392f
};

static const float k_control_mean = -0.007702636711179145f;
static const float k_control_scale = 9.000000089999999f;
static const float k_control_min = -9.0f;
static const float k_control_max = 9.0f;

static stai_ptr g_network = NULL;
static stai_ptr g_inputs[STAI_NETWORK_IN_NUM] = {NULL};
static stai_ptr g_outputs[STAI_NETWORK_OUT_NUM] = {NULL};
static uint32_t g_last_error = 0U;
static uint32_t g_cycles_per_us = 1U;
static bool g_initialized = false;

static float clampf_local(float value, float minimum, float maximum)
{
    if (value < minimum) {
        return minimum;
    }
    if (value > maximum) {
        return maximum;
    }
    return value;
}

static float wrap_to_pi(float angle)
{
    const float pi = 3.14159265358979323846f;
    const float two_pi = 6.28318530717958647692f;
    float wrapped = fmodf(angle + pi, two_pi);
    if (wrapped < 0.0f) {
        wrapped += two_pi;
    }
    return wrapped - pi;
}

static void dwt_init_and_calibrate(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    __DSB();
    __ISB();

    /* Sincroniza con un cambio de SysTick para medir diez milisegundos. */
    uint32_t tick = HAL_GetTick();
    while (HAL_GetTick() == tick) {
    }

    const uint32_t tick_start = HAL_GetTick();
    const uint32_t cycle_start = DWT->CYCCNT;
    while ((HAL_GetTick() - tick_start) < 10U) {
    }
    const uint32_t elapsed_ms = HAL_GetTick() - tick_start;
    const uint32_t elapsed_cycles = DWT->CYCCNT - cycle_start;

    if ((elapsed_ms > 0U) && (elapsed_cycles >= (elapsed_ms * 1000U))) {
        g_cycles_per_us = elapsed_cycles / (elapsed_ms * 1000U);
    } else {
        uint32_t fallback = SystemCoreClock / 1000000U;
        g_cycles_per_us = (fallback > 0U) ? fallback : 1U;
    }
}

bool MPC_AI_Init(void)
{
    stai_return_code status;
    const char *network_name;

    g_initialized = false;
    g_last_error = 0U;

    /* Mantiene la preparación de memoria/RISAF generada por ST Edge AI. */
    aiPreInitialize();

    network_name = stai_mnetwork_find(NULL, 0);
    if (network_name == NULL) {
        g_last_error = 0xE001U;
        return false;
    }

    status = stai_mnetwork_init(network_name, &g_network);
    if (status != STAI_SUCCESS) {
        g_last_error = (uint32_t)status;
        return false;
    }

    status = stai_mnetwork_set_activations(g_network, data_activations);
    if (status != STAI_SUCCESS) {
        g_last_error = (uint32_t)status;
        return false;
    }

    status = stai_mnetwork_get_inputs(g_network, g_inputs);
    if ((status != STAI_SUCCESS) || (g_inputs[0] == NULL)) {
        g_last_error = (status != STAI_SUCCESS) ? (uint32_t)status : 0xE002U;
        return false;
    }

    status = stai_mnetwork_get_outputs(g_network, g_outputs);
    if ((status != STAI_SUCCESS) || (g_outputs[0] == NULL)) {
        g_last_error = (status != STAI_SUCCESS) ? (uint32_t)status : 0xE003U;
        return false;
    }

    dwt_init_and_calibrate();
    g_initialized = true;
    return true;
}

bool MPC_AI_Run(const float state_phys[4],
                float *control_v,
                float *output_normalized,
                uint32_t *latency_us,
                uint32_t *cycles)
{
    if ((!g_initialized) || (state_phys == NULL) || (control_v == NULL)) {
        g_last_error = 0xE010U;
        return false;
    }

    float state_for_model[4] = {
        state_phys[0],
        wrap_to_pi(state_phys[1]),
        state_phys[2],
        state_phys[3]
    };
    float normalized[4];

    for (uint32_t i = 0U; i < 4U; ++i) {
        if (!isfinite(state_for_model[i])) {
            g_last_error = 0xE011U;
            return false;
        }
        normalized[i] = (state_for_model[i] - k_state_mean[i]) / k_state_scale[i];
    }

    memcpy(g_inputs[0], normalized, STAI_NETWORK_IN_1_SIZE_BYTES);

    __DSB();
    const uint32_t cycle_start = DWT->CYCCNT;
    const stai_return_code status = stai_mnetwork_run(g_network);
    const uint32_t elapsed_cycles = DWT->CYCCNT - cycle_start;
    __DSB();

    if (status != STAI_SUCCESS) {
        g_last_error = (uint32_t)status;
        return false;
    }

    float network_output;
    memcpy(&network_output, g_outputs[0], sizeof(network_output));
    if (!isfinite(network_output)) {
        g_last_error = 0xE012U;
        return false;
    }

    *control_v = clampf_local(network_output * k_control_scale + k_control_mean,
                              k_control_min,
                              k_control_max);

    if (output_normalized != NULL) {
        *output_normalized = network_output;
    }
    if (cycles != NULL) {
        *cycles = elapsed_cycles;
    }
    if (latency_us != NULL) {
        *latency_us = elapsed_cycles / g_cycles_per_us;
    }

    g_last_error = 0U;
    return true;
}

uint32_t MPC_AI_GetLastError(void)
{
    return g_last_error;
}

uint32_t MPC_AI_GetCyclesPerMicrosecond(void)
{
    return g_cycles_per_us;
}
