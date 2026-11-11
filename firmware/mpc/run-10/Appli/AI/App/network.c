/**
  ******************************************************************************
  * @file    network.c
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-08-24T15:47:08-0400
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "ai_lite_inspect.h"
#include "ai_platform_interface.h"
#include "layers.h"
#include "core_convert.h"
#include "network.h"
#include "network_details.h"
#include "network_data.h"
#include "stai_events.h"

#include "ai_lite_inspect.h"

#include "lite_operators.h"
/*****************************************************************************/
#define STAI_INTERNAL_API_MAJOR               (1)
#define STAI_INTERNAL_API_MINOR               (0)
#define STAI_INTERNAL_API_MICRO               (0)

#define STAI_MAGIC                            (0xB1C00100)

/*****************************************************************************/
#define _STAI_CONCAT_ARG(a, b)     a ## b
#define STAI_CONCAT(a, b)         _STAI_CONCAT_ARG(a, b)

/*!  STAI_CAST SECTION                       *********************************/
#define STAI_CAST(type, expr) \
  ((type)(expr))


/*****************************************************************************/
#define STAI_SIZE(_size) \
  ((stai_size)(_size))

/*****************************************************************************/
#define STAI_INIT_BUFFER(_flags, _size, _address) \
  { \
    .size = (_size), \
    .address = (uintptr_t)(_address), \
    .flags = (_flags), \
  }

#define STAI_INIT_TENSOR(_name, _flags, _fmt, _size_bytes, _shape, _scale, _zeropoint) \
  { \
    .size_bytes = (_size_bytes), \
    .flags = (_flags), \
    .format = (stai_format)(_fmt), \
    .shape = STAI_PACK(_shape), \
    .scale = STAI_PACK(_scale), \
    .zeropoint = STAI_PACK(_zeropoint), \
    .name = (_name) \
  }

#define STAI_INIT_ARRAY(_size, _ptr) \
  { .size = STAI_SIZE(_size), .data = STAI_PACK(_ptr) }


#define STAI_CAST_ARRAY(_type, _size, _ptr) \
  { .size = STAI_SIZE(_size), .data = (_type)STAI_PACK(_ptr) }


#define STAI_DECLARE_ARRAY(_type, _size, ...) \
  { .size = STAI_SIZE(_size), .data = (_type[_size]) { STAI_PACK(__VA_ARGS__) } }


#define STAI_EMPTY_ARRAY() \
  { .size = 0, .data = NULL }


#define STAI_INIT_VERSION(_major, _minor, _micro) \
  { .major = (_major), .minor = (_minor), .micro = (_micro), .reserved = 0x0 }

/*****************************************************************************/
/**  Getters and setters  **/

#define STAI_GET_ARRAY_SIZE(nd_array) \
  (nd_array.size)


#define STAI_GET_ARRAY_ELEM(nd_array, pos) \
  (nd_array.data[(pos)])

#define _STAI_SET_ERROR(net_ctx, cond, value, exit) { \
  if (!(net_ctx)) { return STAI_ERROR_NETWORK_INVALID_CONTEXT_HANDLE; } \
  if (((uintptr_t)net_ctx) & (_STAI_CONTEXT_ALIGNMENT-1)) { return STAI_ERROR_NETWORK_INVALID_CONTEXT_ALIGNMENT; } \
  if (((value) >= STAI_ERROR_GENERIC) && (cond)) { \
    if ((net_ctx)->_return_code == STAI_SUCCESS) { \
      (net_ctx)->_return_code = (value); \
    } \
    return (exit); \
  } \
}

/*****************************************************************************/
/* TODO REMOVE THESE TWO MACROS */
#define STAI_EVENT_NODE_START_CB
#define STAI_EVENT_NODE_STOP_CB

#ifdef STAI_EVENT_NODE_START_CB
#ifndef _STAI_NETWORK_EVENT_NODE_START_CB
  #define _STAI_NETWORK_EVENT_NODE_START_CB(_node_id, _buffers_size, ...) \
  if (net_ctx->_callback) { \
    const stai_event_node_start_stop _start_event = { \
      .node_id=(_node_id), \
      .buffers={ \
        .size=(_buffers_size), \
        .data=(stai_ptr const*)(const stai_ptr[_buffers_size])STAI_PACK(__VA_ARGS__) \
      } \
    }; \
    net_ctx->_callback(net_ctx->_callback_cookie, STAI_EVENT_NODE_START, (const void*)&_start_event); \
  }
#endif
#else
  #define _STAI_NETWORK_EVENT_NODE_START_CB(_node_id, _buffers_size, ...) \
    do { /* _STAI_NETWORK_EVENT_NODE_START_CB() */ } while(0);
#endif      /* STAI_EVENT_NODE_START_CB */

#ifdef STAI_EVENT_NODE_STOP_CB
#ifndef _STAI_NETWORK_EVENT_NODE_STOP_CB
  #define _STAI_NETWORK_EVENT_NODE_STOP_CB(_node_id, _buffers_size, ...) \
  if (net_ctx->_callback) { \
    const stai_event_node_start_stop _stop_event = { \
      .node_id=(_node_id), \
      .buffers={ \
        .size=(_buffers_size), \
        .data=(stai_ptr const*)(stai_ptr[_buffers_size])STAI_PACK(__VA_ARGS__) \
      } \
    }; \
    net_ctx->_callback(net_ctx->_callback_cookie, STAI_EVENT_NODE_STOP, (const void*)&_stop_event); \
  }
#endif
#else
  #define _STAI_NETWORK_EVENT_NODE_STOP_CB(_node_id, _buffers_size, ...) \
    do { /* _STAI_NETWORK_EVENT_NODE_STOP_CB() */ } while(0);
#endif      /* STAI_EVENT_NODE_STOP_CB */


/*****************************************************************************/
#define _STAI_NETWORK_MODEL_SIGNATURE     "0xcee2ba2777f72e6840ad1f6493d834c7"
#define _STAI_NETWORK_DATETIME            "2026-08-24T15:47:08-0400"
#define _STAI_NETWORK_COMPILE_DATETIME    __DATE__ " " __TIME__

#define _STAI_CONTEXT_ALIGNMENT        STAI_NETWORK_CONTEXT_ALIGNMENT

/*****************************************************************************/
#define g_network_activations_1     (NULL)




#if defined(HAVE_NETWORK_INFO)
/*****************************************************************************/
static const stai_network_info g_network_info = {
  .model_signature = _STAI_NETWORK_MODEL_SIGNATURE,
  .c_compile_datetime = _STAI_NETWORK_COMPILE_DATETIME,
  .c_model_name = STAI_NETWORK_MODEL_NAME,
  .c_model_datetime = _STAI_NETWORK_DATETIME,
  .c_model_signature = 0x0,
  .runtime_version = STAI_INIT_VERSION(12, 0, 1),
  .tool_version = STAI_INIT_VERSION(4, 0, 1),
  .api_version = STAI_INIT_VERSION(1, 0, 0),
  .n_macc = STAI_NETWORK_MACC_NUM,
  .n_nodes = STAI_NETWORK_NODES_NUM,
  .flags = STAI_NETWORK_FLAGS,
  .n_inputs = STAI_NETWORK_IN_NUM,
  .n_outputs = STAI_NETWORK_OUT_NUM,
  .n_activations = STAI_NETWORK_ACTIVATIONS_NUM,
  .n_weights = STAI_NETWORK_WEIGHTS_NUM,
  .n_states = STAI_NETWORK_STATES_NUM,
  .inputs = (stai_tensor[STAI_NETWORK_IN_NUM]) {
    STAI_INIT_TENSOR(
      STAI_NETWORK_IN_1_NAME,
      STAI_NETWORK_IN_1_FLAGS,
      STAI_NETWORK_IN_1_FORMAT,
      STAI_NETWORK_IN_1_SIZE_BYTES,
      STAI_DECLARE_ARRAY(int32_t, 2, 1, 4),
      STAI_EMPTY_ARRAY(),
      STAI_EMPTY_ARRAY()),
    },
    .outputs = (stai_tensor[STAI_NETWORK_OUT_NUM]) {
    STAI_INIT_TENSOR(
      STAI_NETWORK_OUT_1_NAME,
      STAI_NETWORK_OUT_1_FLAGS,
      STAI_NETWORK_OUT_1_FORMAT,
      STAI_NETWORK_OUT_1_SIZE_BYTES,
      STAI_DECLARE_ARRAY(int32_t, 2, 1, 1),
      STAI_EMPTY_ARRAY(),
      STAI_EMPTY_ARRAY()),
    },
  .activations = (stai_tensor[STAI_NETWORK_ACTIVATIONS_NUM]) {
    STAI_INIT_TENSOR(
      (NULL),
      STAI_NETWORK_ACTIVATION_1_FLAGS,
      STAI_FORMAT_U8,
      STAI_NETWORK_ACTIVATION_1_SIZE_BYTES,
      STAI_DECLARE_ARRAY(int32_t, 1, 852),
      STAI_EMPTY_ARRAY(),
      STAI_EMPTY_ARRAY()),
    },
  .weights = (stai_tensor[STAI_NETWORK_WEIGHTS_NUM]) {
    STAI_INIT_TENSOR(
      (NULL),
      STAI_NETWORK_WEIGHT_1_FLAGS,
      STAI_FORMAT_U8,
      STAI_NETWORK_WEIGHT_1_SIZE_BYTES,
      STAI_DECLARE_ARRAY(int32_t, 1, 47404),
      STAI_EMPTY_ARRAY(),
      STAI_EMPTY_ARRAY()),
    },

  .states = NULL
};
#endif

#define _STAI_CONTEXT_ACQUIRE(_net_ctx, _net_handle) \
  _stai_network_context* _net_ctx = (_stai_network_context*)(_net_handle); \
  STAI_ASSERT(_net_ctx != NULL) \
  _STAI_SET_ERROR(_net_ctx, _net_ctx->_magic != STAI_MAGIC, \
                  STAI_ERROR_NETWORK_INVALID_CONTEXT_HANDLE, _net_ctx->_return_code)


/*****************************************************************************/
static
void _stai_network_check(_stai_network_context* net_ctx)
{
  stai_size idx;

// Check activations status
  for (idx=0; idx<STAI_NETWORK_ACTIVATIONS_NUM; idx++) {
    if (net_ctx->_activations[idx] == NULL) break;
  }
  net_ctx->_flags |= (idx == STAI_NETWORK_ACTIVATIONS_NUM) ? STAI_FLAG_ACTIVATIONS : STAI_FLAG_NONE;
// Check inputs status
  for (idx=0; idx<STAI_NETWORK_IN_NUM; idx++) {
    if (net_ctx->_inputs[idx] == NULL) break;
  }
  net_ctx->_flags |= (idx == STAI_NETWORK_IN_NUM) ? STAI_FLAG_INPUTS : STAI_FLAG_NONE;

  // Check outputs status
  for (idx=0; idx<STAI_NETWORK_OUT_NUM; idx++) {
    if (net_ctx->_outputs[idx] == NULL) break;
  }
  net_ctx->_flags |= (idx == STAI_NETWORK_OUT_NUM) ? STAI_FLAG_OUTPUTS : STAI_FLAG_NONE;

// Check weights status
  for (idx=0; idx<STAI_NETWORK_WEIGHTS_NUM; idx++) {
    if (net_ctx->_weights[idx] == NULL) break;
  }
  net_ctx->_flags |= (idx == STAI_NETWORK_WEIGHTS_NUM) ? STAI_FLAG_WEIGHTS : STAI_FLAG_NONE;
STAI_PRINT("  [_stai_network_check] flags: 0x%08x\n", net_ctx->_flags)
}


/*****************************************************************************/
STAI_API_ENTRY
stai_return_code stai_network_init(
  stai_network* network)
{
  /* Memory where to store internal context is provided by applications as a raw byte buffer */
  _stai_network_context* net_ctx = (_stai_network_context*)(network);
  net_ctx->_return_code = STAI_SUCCESS;
  STAI_PRINT("[Entering Network Init] network(%p) context_size(%d)\n", net_ctx, (int32_t)sizeof(_stai_network_context))

  _STAI_SET_ERROR(net_ctx, STAI_NETWORK_CONTEXT_SIZE != sizeof(_stai_network_context),
                 STAI_ERROR_NETWORK_INVALID_CONTEXT_SIZE, net_ctx->_return_code)

  {
    const _stai_network_context _network_context = {
      ._magic = STAI_MAGIC,
      ._signature = STAI_NETWORK_MODEL_SIGNATURE,
      ._flags = STAI_NETWORK_FLAGS,
      ._return_code = STAI_SUCCESS,
      ._callback = NULL,
      ._callback_cookie = NULL,
      ._activations = {
      (stai_ptr)g_network_activations_1
      },
      ._weights = {
      (stai_ptr)g_network_weights_array
      },
      ._inputs = {
    NULL},
      ._outputs = {
    NULL},
    };

    // Deep copy of internal context to opaque buffer provided by app
    *net_ctx = _network_context;

    _stai_network_check(net_ctx);
  }

  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_deinit(
  stai_network* network)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  /*  Reset flags to initial state  */
  net_ctx->_flags = STAI_NETWORK_FLAGS;
  return net_ctx->_return_code;
}

/*****************************************************************************/



/* Int quant #0 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conversion_2_output_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0078125f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #1 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_3_output_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.08926830440759659f),
    AI_PACK_INTQ_ZP(-2)))

/* Int quant #2 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_3_weights_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 50,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.001743961009196937f, 0.013590412214398384f, 0.0012270667357370257f, 0.009768908843398094f, 0.008524710312485695f, 0.0013090422144159675f, 0.02025660313665867f, 0.012407415546476841f, 0.010214246809482574f, 0.012172735296189785f, 0.001826795400120318f, 0.0014256261056289077f, 0.011383743956685066f, 0.0018120643217116594f, 0.013280673883855343f, 0.010319596156477928f, 0.01195567287504673f, 0.016710324212908745f, 0.001414693077094853f, 0.011883018538355827f, 0.0014591022627428174f, 0.0013716499088332057f, 0.01024205144494772f, 0.01365385390818119f, 0.009149538353085518f, 0.012782580219209194f, 0.02823920175433159f, 0.013580991886556149f, 0.012712961062788963f, 0.0030013404320925474f, 0.011062784120440483f, 0.0019458724418655038f, 0.014134285040199757f, 0.010614541359245777f, 0.00145729910582304f, 0.021635752171278f, 0.008670841343700886f, 0.006405691150575876f, 0.002092207781970501f, 0.013505828566849232f, 0.009196468628942966f, 0.009344552643597126f, 0.0020181427244096994f, 0.01691681146621704f, 0.001320393173955381f, 0.006853023078292608f, 0.006620949134230614f, 0.011048441752791405f, 0.010448207147419453f, 0.014363438822329044f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #3 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(nl_4_output_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0078125f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #4 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conversion_14_output_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0078125f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #5 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_15_output_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0559731125831604f),
    AI_PACK_INTQ_ZP(-5)))

/* Int quant #6 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_15_weights_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 50,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0002762539079412818f, 0.0003368390607647598f, 0.008061851374804974f, 0.0071817487478256226f, 0.000286544905975461f, 0.00032862788066267967f, 0.0002214181877207011f, 0.0003548343956936151f, 0.0002953743387479335f, 0.00020912133913952857f, 0.00026349470135755837f, 0.00020335330918896943f, 0.007572168484330177f, 0.00024332875909749418f, 0.004633454605937004f, 0.0004325231129769236f, 0.0005408226861618459f, 0.0002346647233935073f, 0.0032797292806208134f, 0.011414594948291779f, 0.005080755800008774f, 0.005396427121013403f, 0.003215029602870345f, 0.0002980664139613509f, 0.0005813632742501795f, 0.0004163775302004069f, 0.00027461309218779206f, 0.0002695576404221356f, 0.00033389535383321345f, 0.00020037325157318264f, 0.00022794160759076476f, 0.0002710161788854748f, 0.0003269924782216549f, 0.0003214903990738094f, 0.00026928391889669f, 0.00034379848511889577f, 0.009956084191799164f, 0.00029513458139263093f, 0.0004107927961740643f, 0.0003745609719771892f, 0.005301897879689932f, 0.0002712064888328314f, 0.00024580315221101046f, 0.003956225235015154f, 0.005500886123627424f, 0.00041393120773136616f, 0.0002516953682061285f, 0.0003160872729495168f, 0.00027555282576940954f, 0.00555312680080533f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #7 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(nl_16_output_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0078125f),
    AI_PACK_INTQ_ZP(0)))



/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  conversion_2_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 50, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  gemm_3_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 50, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  gemm_3_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2500, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  gemm_3_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 50, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  gemm_3_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 300, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  nl_4_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 50, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  conversion_14_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 50, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  gemm_15_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 50, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  gemm_15_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2500, AI_STATIC)

/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  gemm_15_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 50, AI_STATIC)

/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  gemm_15_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 300, AI_STATIC)

/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  nl_16_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 50, AI_STATIC)



/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  conversion_2_output, AI_STATIC,
  2, 0x1,
  AI_SHAPE_INIT(4, 1, 50, 1, 1), AI_STRIDE_INIT(4, 1, 1, 50, 50),
  1, &conversion_2_output_array, &conversion_2_output_array_intq)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  gemm_3_bias, AI_STATIC,
  20, 0x0,
  AI_SHAPE_INIT(4, 1, 50, 1, 1), AI_STRIDE_INIT(4, 4, 4, 200, 200),
  1, &gemm_3_bias_array, NULL)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  gemm_3_output, AI_STATIC,
  21, 0x1,
  AI_SHAPE_INIT(4, 1, 50, 1, 1), AI_STRIDE_INIT(4, 1, 1, 50, 50),
  1, &gemm_3_output_array, &gemm_3_output_array_intq)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  gemm_3_scratch0, AI_STATIC,
  22, 0x0,
  AI_SHAPE_INIT(4, 1, 300, 1, 1), AI_STRIDE_INIT(4, 2, 2, 600, 600),
  1, &gemm_3_scratch0_array, NULL)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  gemm_3_weights, AI_STATIC,
  23, 0x1,
  AI_SHAPE_INIT(4, 50, 50, 1, 1), AI_STRIDE_INIT(4, 1, 50, 2500, 2500),
  1, &gemm_3_weights_array, &gemm_3_weights_array_intq)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  nl_4_output, AI_STATIC,
  34, 0x1,
  AI_SHAPE_INIT(4, 1, 50, 1, 1), AI_STRIDE_INIT(4, 1, 1, 50, 50),
  1, &nl_4_output_array, &nl_4_output_array_intq)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  conversion_14_output, AI_STATIC,
  0, 0x1,
  AI_SHAPE_INIT(4, 1, 50, 1, 1), AI_STRIDE_INIT(4, 1, 1, 50, 50),
  1, &conversion_14_output_array, &conversion_14_output_array_intq)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  gemm_15_bias, AI_STATIC,
  13, 0x0,
  AI_SHAPE_INIT(4, 1, 50, 1, 1), AI_STRIDE_INIT(4, 4, 4, 200, 200),
  1, &gemm_15_bias_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  gemm_15_output, AI_STATIC,
  14, 0x1,
  AI_SHAPE_INIT(4, 1, 50, 1, 1), AI_STRIDE_INIT(4, 1, 1, 50, 50),
  1, &gemm_15_output_array, &gemm_15_output_array_intq)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  gemm_15_scratch0, AI_STATIC,
  15, 0x0,
  AI_SHAPE_INIT(4, 1, 300, 1, 1), AI_STRIDE_INIT(4, 2, 2, 600, 600),
  1, &gemm_15_scratch0_array, NULL)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  gemm_15_weights, AI_STATIC,
  16, 0x1,
  AI_SHAPE_INIT(4, 50, 50, 1, 1), AI_STRIDE_INIT(4, 1, 50, 2500, 2500),
  1, &gemm_15_weights_array, &gemm_15_weights_array_intq)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  nl_16_output, AI_STATIC,
  32, 0x1,
  AI_SHAPE_INIT(4, 1, 50, 1, 1), AI_STRIDE_INIT(4, 1, 1, 50, 50),
  1, &nl_16_output_array, &nl_16_output_array_intq)


AI_TENSOR_CHAIN_OBJ_DECLARE(
  gemm_3_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conversion_2_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_3_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &gemm_3_weights, &gemm_3_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_3_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  gemm_3_layer, 3,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA_ch,
  &gemm_3_chain,
  NULL, &gemm_3_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i8 nl_4_nl_params_data[] = { -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -127, -127, -127, -127, -127, -127, -126, -126, -126, -125, -125, -124, -123, -122, -121, -120, -118, -116, -114, -112, -109, -105, -101, -97, -91, -85, -78, -71, -63, -54, -44, -33, -23, -11, 0, 11, 23, 33, 44, 54, 63, 71, 78, 85, 91, 97, 101, 105, 109, 112, 114, 116, 118, 120, 121, 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    nl_4_nl_params, AI_ARRAY_FORMAT_S8,
    nl_4_nl_params_data, nl_4_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_4_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_3_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_4_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_4_layer, 4,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &nl_4_chain,
  NULL, &nl_4_layer, AI_STATIC, 
  .nl_params = &nl_4_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  gemm_15_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conversion_14_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_15_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &gemm_15_weights, &gemm_15_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_15_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  gemm_15_layer, 15,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA_ch,
  &gemm_15_chain,
  NULL, &gemm_15_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i8 nl_16_nl_params_data[] = { -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -126, -126, -126, -126, -125, -125, -125, -124, -124, -124, -123, -122, -122, -121, -120, -119, -118, -117, -116, -115, -113, -112, -110, -108, -106, -103, -101, -98, -95, -91, -88, -84, -80, -75, -70, -65, -60, -54, -48, -41, -35, -28, -21, -14, -7, 0, 7, 14, 21, 28, 35, 41, 48, 54, 60, 65, 70, 75, 80, 84, 88, 91, 95, 98, 101, 103, 106, 108, 110, 112, 113, 115, 116, 117, 118, 119, 120, 121, 122, 122, 123, 124, 124, 124, 125, 125, 125, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    nl_16_nl_params, AI_ARRAY_FORMAT_S8,
    nl_16_nl_params_data, nl_16_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_16_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_15_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_16_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_16_layer, 16,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &nl_16_chain,
  NULL, &nl_16_layer, AI_STATIC, 
  .nl_params = &nl_16_nl_params, 
)
/**  Hybrid layers declarations section  *************************************/
void forward_lite_dense_integer_SSSA_ch_gemm_3(_stai_network_context* net_ctx)
{
  conversion_2_output_array.data = AI_PTR(net_ctx->_activations[0] + 800);
  conversion_2_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 800);
  gemm_3_weights_array.data = AI_PTR(net_ctx->_weights[0] + 1000);
  gemm_3_weights_array.data_start = AI_PTR(net_ctx->_weights[0] + 1000);
  gemm_3_bias_array.data = AI_PTR(net_ctx->_weights[0] + 3500);
  gemm_3_bias_array.data_start = AI_PTR(net_ctx->_weights[0] + 3500);
  gemm_3_scratch0_array.data = AI_PTR(net_ctx->_activations[0] + 0);
  gemm_3_scratch0_array.data_start = AI_PTR(net_ctx->_activations[0] + 0);
  gemm_3_output_array.data = AI_PTR(net_ctx->_activations[0] + 748);
  gemm_3_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 748);
  _STAI_NETWORK_EVENT_NODE_START_CB(3, 1, { conversion_2_output.data->data});
  forward_dense_integer_SSSA_ch(&gemm_3_layer);
  _STAI_NETWORK_EVENT_NODE_STOP_CB(3, 1, { gemm_3_output.data->data});
}
void forward_lite_nl_integer_nl_4(_stai_network_context* net_ctx)
{
  gemm_3_output_array.data = AI_PTR(net_ctx->_activations[0] + 748);
  gemm_3_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 748);
  nl_4_output_array.data = AI_PTR(net_ctx->_activations[0] + 800);
  nl_4_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 800);
  _STAI_NETWORK_EVENT_NODE_START_CB(4, 1, { gemm_3_output.data->data});
  forward_nl_integer(&nl_4_layer);
  _STAI_NETWORK_EVENT_NODE_STOP_CB(4, 1, { nl_4_output.data->data});
}
void forward_lite_dense_integer_SSSA_ch_gemm_15(_stai_network_context* net_ctx)
{
  conversion_14_output_array.data = AI_PTR(net_ctx->_activations[0] + 800);
  conversion_14_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 800);
  gemm_15_weights_array.data = AI_PTR(net_ctx->_weights[0] + 44500);
  gemm_15_weights_array.data_start = AI_PTR(net_ctx->_weights[0] + 44500);
  gemm_15_bias_array.data = AI_PTR(net_ctx->_weights[0] + 47000);
  gemm_15_bias_array.data_start = AI_PTR(net_ctx->_weights[0] + 47000);
  gemm_15_scratch0_array.data = AI_PTR(net_ctx->_activations[0] + 0);
  gemm_15_scratch0_array.data_start = AI_PTR(net_ctx->_activations[0] + 0);
  gemm_15_output_array.data = AI_PTR(net_ctx->_activations[0] + 600);
  gemm_15_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 600);
  _STAI_NETWORK_EVENT_NODE_START_CB(15, 1, { conversion_14_output.data->data});
  forward_dense_integer_SSSA_ch(&gemm_15_layer);
  _STAI_NETWORK_EVENT_NODE_STOP_CB(15, 1, { gemm_15_output.data->data});
}
void forward_lite_nl_integer_nl_16(_stai_network_context* net_ctx)
{
  gemm_15_output_array.data = AI_PTR(net_ctx->_activations[0] + 600);
  gemm_15_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 600);
  nl_16_output_array.data = AI_PTR(net_ctx->_activations[0] + 800);
  nl_16_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 800);
  _STAI_NETWORK_EVENT_NODE_START_CB(16, 1, { gemm_15_output.data->data});
  forward_nl_integer(&nl_16_layer);
  _STAI_NETWORK_EVENT_NODE_STOP_CB(16, 1, { nl_16_output.data->data});
}

/*****************************************************************************/



static const ai_i32 nl_1_t_in_0_shape_ch_prod_const_s32 = 50;

static const ai_u32 conversion_2_t_out_0_shape_h_w_ch_d_prod_const_u32 = 50;
static const ai_float conversion_2_t_out_0_fmt_scale_const_f32 = 0.0078125f;
static const ai_i8 conversion_2_t_out_0_fmt_zero_const_s8 = 0;



static const ai_u32 conversion_5_t_out_0_shape_h_w_ch_d_prod_const_u32 = 50;
static const ai_float conversion_5_t_in_0_fmt_scale_const_f32 = 0.0078125f;
static const ai_i8 conversion_5_t_in_0_fmt_zero_const_s8 = 0;


static const ai_i32 nl_7_t_in_0_shape_ch_prod_const_s32 = 50;


static const ai_i32 nl_9_t_in_0_shape_ch_prod_const_s32 = 50;


static const ai_i32 nl_11_t_in_0_shape_ch_prod_const_s32 = 50;


static const ai_i32 nl_13_t_in_0_shape_ch_prod_const_s32 = 50;

static const ai_u32 conversion_14_t_out_0_shape_h_w_ch_d_prod_const_u32 = 50;
static const ai_float conversion_14_t_out_0_fmt_scale_const_f32 = 0.0078125f;
static const ai_i8 conversion_14_t_out_0_fmt_zero_const_s8 = 0;



static const ai_u32 conversion_17_t_out_0_shape_h_w_ch_d_prod_const_u32 = 50;
static const ai_float conversion_17_t_in_0_fmt_scale_const_f32 = 0.0078125f;
static const ai_i8 conversion_17_t_in_0_fmt_zero_const_s8 = 0;

STAI_API_ENTRY
stai_return_code stai_network_run(
  stai_network* network,
  const stai_run_mode mode)
{
   STAI_UNUSED(mode)
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  _STAI_SET_ERROR(net_ctx, (net_ctx->_flags & STAI_FLAG_ACTIVATIONS) != STAI_FLAG_ACTIVATIONS,
        STAI_ERROR_NETWORK_INVALID_ACTIVATIONS_PTR, net_ctx->_return_code)

  _STAI_SET_ERROR(net_ctx, (net_ctx->_flags & STAI_FLAG_INPUTS) != STAI_FLAG_INPUTS,
                  STAI_ERROR_NETWORK_INVALID_IN_PTR, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, (net_ctx->_flags & STAI_FLAG_OUTPUTS) != STAI_FLAG_OUTPUTS,
                  STAI_ERROR_NETWORK_INVALID_OUT_PTR, net_ctx->_return_code)

  _STAI_SET_ERROR(net_ctx, (net_ctx->_flags & STAI_FLAG_WEIGHTS) != STAI_FLAG_WEIGHTS,
                  STAI_ERROR_NETWORK_INVALID_WEIGHTS_PTR, net_ctx->_return_code)


  /* LITE_KERNEL_SECTION BEGIN gemm_0 */
  {
      forward_lite_dense_if32of32wf32_args arg_30f51e = {
      .output = (float*)(net_ctx->_activations[0] + 600),
      .input = (float*)(net_ctx->_inputs[0] + 0),
      .weights = (float*)(net_ctx->_weights[0] + 0),
      .bias = (float*)(net_ctx->_weights[0] + 800),
      .n_channel_in = 4,
      .n_channel_out = 50,
      .n_elements = 1,
    };
  
  _STAI_NETWORK_EVENT_NODE_START_CB(0, 1, {(stai_ptr) (float*)(net_ctx->_inputs[0] + 0)});
    
  forward_lite_dense_if32of32wf32((forward_lite_dense_if32of32wf32_args*)&arg_30f51e);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(0, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 600)});
  }
  /* LITE_KERNEL_SECTION END gemm_0 */
  /* LITE_KERNEL_SECTION BEGIN nl_1 */
  {
      ai_handle nl_1_t_out_0_ptr_handle = (ai_handle)(net_ctx->_activations[0] + 600);
    const ai_handle nl_1_t_in_0_ptr_const_handle = (ai_handle)(net_ctx->_activations[0] + 600);
  
  _STAI_NETWORK_EVENT_NODE_START_CB(1, 1, {(stai_ptr) nl_1_t_in_0_ptr_const_handle});
    
  forward_lite_nl_tanh_if32of32(nl_1_t_out_0_ptr_handle, nl_1_t_in_0_ptr_const_handle, nl_1_t_in_0_shape_ch_prod_const_s32, NULL);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(1, 1, {(stai_ptr) nl_1_t_out_0_ptr_handle});
  }
  /* LITE_KERNEL_SECTION END nl_1 */
  /* LITE_KERNEL_SECTION BEGIN conversion_2 */
  {
      const ai_float* conversion_2_t_in_0_ptr_const_f32 = (ai_float*)(net_ctx->_activations[0] + 600);
    ai_i8* conversion_2_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 800);
  
  _STAI_NETWORK_EVENT_NODE_START_CB(2, 1, {(stai_ptr) conversion_2_t_in_0_ptr_const_f32});
    
  forward_lite_node_convert_integer_if32os8(conversion_2_t_in_0_ptr_const_f32, conversion_2_t_out_0_ptr_s8, conversion_2_t_out_0_shape_h_w_ch_d_prod_const_u32, conversion_2_t_out_0_fmt_scale_const_f32, conversion_2_t_out_0_fmt_zero_const_s8);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(2, 1, {(stai_ptr) conversion_2_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END conversion_2 */
  /* LITE_KERNEL_SECTION BEGIN gemm_3 */
  {
    
  forward_lite_dense_integer_SSSA_ch_gemm_3(net_ctx);
  }
  /* LITE_KERNEL_SECTION END gemm_3 */
  /* LITE_KERNEL_SECTION BEGIN nl_4 */
  {
    
  forward_lite_nl_integer_nl_4(net_ctx);
  }
  /* LITE_KERNEL_SECTION END nl_4 */
  /* LITE_KERNEL_SECTION BEGIN conversion_5 */
  {
      const ai_i8* conversion_5_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 800);
    ai_float* conversion_5_t_out_0_ptr_f32 = (ai_float*)(net_ctx->_activations[0] + 648);
  
  _STAI_NETWORK_EVENT_NODE_START_CB(5, 1, {(stai_ptr) conversion_5_t_in_0_ptr_const_s8});
    
  forward_lite_node_convert_integer_is8of32(conversion_5_t_in_0_ptr_const_s8, conversion_5_t_out_0_ptr_f32, conversion_5_t_out_0_shape_h_w_ch_d_prod_const_u32, conversion_5_t_in_0_fmt_scale_const_f32, conversion_5_t_in_0_fmt_zero_const_s8);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(5, 1, {(stai_ptr) conversion_5_t_out_0_ptr_f32});
  }
  /* LITE_KERNEL_SECTION END conversion_5 */
  /* LITE_KERNEL_SECTION BEGIN gemm_6 */
  {
      forward_lite_dense_if32of32wf32_args arg_30f51e = {
      .output = (float*)(net_ctx->_activations[0] + 0),
      .input = (float*)(net_ctx->_activations[0] + 648),
      .weights = (float*)(net_ctx->_weights[0] + 3700),
      .bias = (float*)(net_ctx->_weights[0] + 13700),
      .n_channel_in = 50,
      .n_channel_out = 50,
      .n_elements = 1,
    };
  
  _STAI_NETWORK_EVENT_NODE_START_CB(6, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 648)});
    
  forward_lite_dense_if32of32wf32((forward_lite_dense_if32of32wf32_args*)&arg_30f51e);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(6, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 0)});
  }
  /* LITE_KERNEL_SECTION END gemm_6 */
  /* LITE_KERNEL_SECTION BEGIN nl_7 */
  {
      ai_handle nl_7_t_out_0_ptr_handle = (ai_handle)(net_ctx->_activations[0] + 648);
    const ai_handle nl_7_t_in_0_ptr_const_handle = (ai_handle)(net_ctx->_activations[0] + 0);
  
  _STAI_NETWORK_EVENT_NODE_START_CB(7, 1, {(stai_ptr) nl_7_t_in_0_ptr_const_handle});
    
  forward_lite_nl_tanh_if32of32(nl_7_t_out_0_ptr_handle, nl_7_t_in_0_ptr_const_handle, nl_7_t_in_0_shape_ch_prod_const_s32, NULL);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(7, 1, {(stai_ptr) nl_7_t_out_0_ptr_handle});
  }
  /* LITE_KERNEL_SECTION END nl_7 */
  /* LITE_KERNEL_SECTION BEGIN gemm_8 */
  {
      forward_lite_dense_if32of32wf32_args arg_30f51e = {
      .output = (float*)(net_ctx->_activations[0] + 0),
      .input = (float*)(net_ctx->_activations[0] + 648),
      .weights = (float*)(net_ctx->_weights[0] + 13900),
      .bias = (float*)(net_ctx->_weights[0] + 23900),
      .n_channel_in = 50,
      .n_channel_out = 50,
      .n_elements = 1,
    };
  
  _STAI_NETWORK_EVENT_NODE_START_CB(8, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 648)});
    
  forward_lite_dense_if32of32wf32((forward_lite_dense_if32of32wf32_args*)&arg_30f51e);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(8, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 0)});
  }
  /* LITE_KERNEL_SECTION END gemm_8 */
  /* LITE_KERNEL_SECTION BEGIN nl_9 */
  {
      ai_handle nl_9_t_out_0_ptr_handle = (ai_handle)(net_ctx->_activations[0] + 648);
    const ai_handle nl_9_t_in_0_ptr_const_handle = (ai_handle)(net_ctx->_activations[0] + 0);
  
  _STAI_NETWORK_EVENT_NODE_START_CB(9, 1, {(stai_ptr) nl_9_t_in_0_ptr_const_handle});
    
  forward_lite_nl_tanh_if32of32(nl_9_t_out_0_ptr_handle, nl_9_t_in_0_ptr_const_handle, nl_9_t_in_0_shape_ch_prod_const_s32, NULL);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(9, 1, {(stai_ptr) nl_9_t_out_0_ptr_handle});
  }
  /* LITE_KERNEL_SECTION END nl_9 */
  /* LITE_KERNEL_SECTION BEGIN gemm_10 */
  {
      forward_lite_dense_if32of32wf32_args arg_30f51e = {
      .output = (float*)(net_ctx->_activations[0] + 0),
      .input = (float*)(net_ctx->_activations[0] + 648),
      .weights = (float*)(net_ctx->_weights[0] + 24100),
      .bias = (float*)(net_ctx->_weights[0] + 34100),
      .n_channel_in = 50,
      .n_channel_out = 50,
      .n_elements = 1,
    };
  
  _STAI_NETWORK_EVENT_NODE_START_CB(10, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 648)});
    
  forward_lite_dense_if32of32wf32((forward_lite_dense_if32of32wf32_args*)&arg_30f51e);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(10, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 0)});
  }
  /* LITE_KERNEL_SECTION END gemm_10 */
  /* LITE_KERNEL_SECTION BEGIN nl_11 */
  {
      ai_handle nl_11_t_out_0_ptr_handle = (ai_handle)(net_ctx->_activations[0] + 648);
    const ai_handle nl_11_t_in_0_ptr_const_handle = (ai_handle)(net_ctx->_activations[0] + 0);
  
  _STAI_NETWORK_EVENT_NODE_START_CB(11, 1, {(stai_ptr) nl_11_t_in_0_ptr_const_handle});
    
  forward_lite_nl_tanh_if32of32(nl_11_t_out_0_ptr_handle, nl_11_t_in_0_ptr_const_handle, nl_11_t_in_0_shape_ch_prod_const_s32, NULL);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(11, 1, {(stai_ptr) nl_11_t_out_0_ptr_handle});
  }
  /* LITE_KERNEL_SECTION END nl_11 */
  /* LITE_KERNEL_SECTION BEGIN gemm_12 */
  {
      forward_lite_dense_if32of32wf32_args arg_30f51e = {
      .output = (float*)(net_ctx->_activations[0] + 0),
      .input = (float*)(net_ctx->_activations[0] + 648),
      .weights = (float*)(net_ctx->_weights[0] + 34300),
      .bias = (float*)(net_ctx->_weights[0] + 44300),
      .n_channel_in = 50,
      .n_channel_out = 50,
      .n_elements = 1,
    };
  
  _STAI_NETWORK_EVENT_NODE_START_CB(12, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 648)});
    
  forward_lite_dense_if32of32wf32((forward_lite_dense_if32of32wf32_args*)&arg_30f51e);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(12, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 0)});
  }
  /* LITE_KERNEL_SECTION END gemm_12 */
  /* LITE_KERNEL_SECTION BEGIN nl_13 */
  {
      ai_handle nl_13_t_out_0_ptr_handle = (ai_handle)(net_ctx->_activations[0] + 600);
    const ai_handle nl_13_t_in_0_ptr_const_handle = (ai_handle)(net_ctx->_activations[0] + 0);
  
  _STAI_NETWORK_EVENT_NODE_START_CB(13, 1, {(stai_ptr) nl_13_t_in_0_ptr_const_handle});
    
  forward_lite_nl_tanh_if32of32(nl_13_t_out_0_ptr_handle, nl_13_t_in_0_ptr_const_handle, nl_13_t_in_0_shape_ch_prod_const_s32, NULL);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(13, 1, {(stai_ptr) nl_13_t_out_0_ptr_handle});
  }
  /* LITE_KERNEL_SECTION END nl_13 */
  /* LITE_KERNEL_SECTION BEGIN conversion_14 */
  {
      const ai_float* conversion_14_t_in_0_ptr_const_f32 = (ai_float*)(net_ctx->_activations[0] + 600);
    ai_i8* conversion_14_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 800);
  
  _STAI_NETWORK_EVENT_NODE_START_CB(14, 1, {(stai_ptr) conversion_14_t_in_0_ptr_const_f32});
    
  forward_lite_node_convert_integer_if32os8(conversion_14_t_in_0_ptr_const_f32, conversion_14_t_out_0_ptr_s8, conversion_14_t_out_0_shape_h_w_ch_d_prod_const_u32, conversion_14_t_out_0_fmt_scale_const_f32, conversion_14_t_out_0_fmt_zero_const_s8);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(14, 1, {(stai_ptr) conversion_14_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END conversion_14 */
  /* LITE_KERNEL_SECTION BEGIN gemm_15 */
  {
    
  forward_lite_dense_integer_SSSA_ch_gemm_15(net_ctx);
  }
  /* LITE_KERNEL_SECTION END gemm_15 */
  /* LITE_KERNEL_SECTION BEGIN nl_16 */
  {
    
  forward_lite_nl_integer_nl_16(net_ctx);
  }
  /* LITE_KERNEL_SECTION END nl_16 */
  /* LITE_KERNEL_SECTION BEGIN conversion_17 */
  {
      const ai_i8* conversion_17_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 800);
    ai_float* conversion_17_t_out_0_ptr_f32 = (ai_float*)(net_ctx->_activations[0] + 600);
  
  _STAI_NETWORK_EVENT_NODE_START_CB(17, 1, {(stai_ptr) conversion_17_t_in_0_ptr_const_s8});
    
  forward_lite_node_convert_integer_is8of32(conversion_17_t_in_0_ptr_const_s8, conversion_17_t_out_0_ptr_f32, conversion_17_t_out_0_shape_h_w_ch_d_prod_const_u32, conversion_17_t_in_0_fmt_scale_const_f32, conversion_17_t_in_0_fmt_zero_const_s8);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(17, 1, {(stai_ptr) conversion_17_t_out_0_ptr_f32});
  }
  /* LITE_KERNEL_SECTION END conversion_17 */
  /* LITE_KERNEL_SECTION BEGIN gemm_18 */
  {
      forward_lite_dense_if32of32wf32_args arg_30f51e = {
      .output = (float*)(net_ctx->_outputs[0] + 0),
      .input = (float*)(net_ctx->_activations[0] + 600),
      .weights = (float*)(net_ctx->_weights[0] + 47200),
      .bias = (float*)(net_ctx->_weights[0] + 47400),
      .n_channel_in = 50,
      .n_channel_out = 1,
      .n_elements = 1,
    };
  
  _STAI_NETWORK_EVENT_NODE_START_CB(18, 1, {(stai_ptr) (float*)(net_ctx->_activations[0] + 600)});
    
  forward_lite_dense_if32of32wf32((forward_lite_dense_if32of32wf32_args*)&arg_30f51e);
    
  _STAI_NETWORK_EVENT_NODE_STOP_CB(18, 1, {(stai_ptr) (float*)(net_ctx->_outputs[0] + 0)});
  }
  /* LITE_KERNEL_SECTION END gemm_18 */
  return net_ctx->_return_code;
}

/*****************************************************************************/
/*  Getters APIs Section  */
STAI_API_ENTRY
stai_size stai_network_get_context_size()
{
  return (stai_size)STAI_NETWORK_CONTEXT_SIZE;
}

#if defined(HAVE_NETWORK_INFO)
STAI_API_ENTRY
stai_return_code stai_network_get_info(
  stai_network* network,
  stai_network_info* info)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, info==NULL, STAI_ERROR_NETWORK_INVALID_INFO, net_ctx->_return_code)

  // Copy of network info struct
  *info = g_network_info;

  return STAI_SUCCESS;
}
#endif


STAI_API_ENTRY
stai_return_code stai_network_get_activations(
  stai_network* network, stai_ptr* activations, stai_size* n_activations)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  _STAI_SET_ERROR(net_ctx, !n_activations, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  *n_activations = STAI_NETWORK_ACTIVATIONS_NUM;
for (stai_size idx=0; activations && (idx<STAI_NETWORK_ACTIVATIONS_NUM); idx++) {
    // get address of the activations buffers
    activations[idx] = net_ctx->_activations[idx];
  }return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_get_weights(
  stai_network* network, stai_ptr* weights, stai_size* n_weights)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !n_weights, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  *n_weights = STAI_NETWORK_WEIGHTS_NUM;
for (stai_size idx=0; weights && (idx<STAI_NETWORK_WEIGHTS_NUM); idx++) {
    // get address of the weights buffers
    weights[idx] = net_ctx->_weights[idx];
  }return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_get_inputs(
  stai_network* network, stai_ptr* inputs, stai_size* n_inputs)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !n_inputs, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  *n_inputs = STAI_NETWORK_IN_NUM;
  for (stai_size idx=0; inputs && (idx<STAI_NETWORK_IN_NUM); idx++) {
    inputs[idx] = net_ctx->_inputs[idx];
  }
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_get_outputs(
  stai_network* network, stai_ptr* outputs, stai_size* n_outputs)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !n_outputs, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  *n_outputs = STAI_NETWORK_OUT_NUM;
  for (stai_size idx=0; outputs && (idx<STAI_NETWORK_OUT_NUM); idx++) {
    outputs[idx] = net_ctx->_outputs[idx];
  }
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_get_error(
  stai_network* network)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  /* return 1st generated error or STAI_SUCCESS if no errors so far */
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_get_states(
  stai_network* network, stai_ptr* states, stai_size* n_states)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !n_states, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  /* get the number of internals states (supporting multi-heap also for internal states) */
  *n_states = STAI_NETWORK_STATES_NUM;

  STAI_UNUSED(states)
return net_ctx->_return_code;
}


/*****************************************************************************/
/*  Setters APIs Section  */

STAI_API_ENTRY
stai_return_code stai_network_set_activations(
  stai_network* network,
  const stai_ptr* activations,
  const stai_size n_activations)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
const uintptr_t _activations_alignment[] = STAI_NETWORK_ACTIVATIONS_ALIGNMENTS;
  STAI_PRINT("  [stai_network_set_activations] network(%p) activations[%d]: %p\n\n", net_ctx, n_activations, activations)
  _STAI_SET_ERROR(net_ctx, !activations,
                  STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, n_activations!=STAI_NETWORK_ACTIVATIONS_NUM,
                  STAI_ERROR_NETWORK_INVALID_ACTIVATIONS_NUM, net_ctx->_return_code)

  for (stai_size idx=0; activations && idx<STAI_NETWORK_ACTIVATIONS_NUM; idx++) {
    STAI_PRINT("  activation[%d]: %p\n", idx, activations[idx])
    _STAI_SET_ERROR(net_ctx, activations[idx]==NULL,
                    STAI_ERROR_NETWORK_INVALID_ACTIVATIONS_PTR, net_ctx->_return_code)
    _STAI_SET_ERROR(net_ctx, ((uintptr_t)activations[idx]) & (_activations_alignment[idx]-1),
                    STAI_ERROR_INVALID_BUFFER_ALIGNMENT, net_ctx->_return_code)
    net_ctx->_activations[idx] = activations[idx];
  }
  net_ctx->_inputs[0] = activations[0] + 800;

  net_ctx->_outputs[0] = activations[0] + 800;
_stai_network_check(net_ctx);
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_set_weights(
  stai_network* network,
  const stai_ptr* weights,
  const stai_size n_weights)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
const uintptr_t _weights_alignment[] = STAI_NETWORK_WEIGHTS_ALIGNMENTS;
  _STAI_SET_ERROR(net_ctx, !weights,
                  STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, n_weights!=STAI_NETWORK_WEIGHTS_NUM,
                  STAI_ERROR_NETWORK_INVALID_WEIGHTS_NUM, net_ctx->_return_code)
  for (stai_size idx=0; weights && idx<STAI_NETWORK_WEIGHTS_NUM; idx++) {
    STAI_PRINT("  weight[%d]: %p\n", idx, weights[idx])
    _STAI_SET_ERROR(net_ctx, weights[idx]==NULL,
                    STAI_ERROR_NETWORK_INVALID_WEIGHTS_PTR, net_ctx->_return_code)
    _STAI_SET_ERROR(net_ctx, ((uintptr_t)weights[idx]) & (_weights_alignment[idx]-1),
                    STAI_ERROR_INVALID_BUFFER_ALIGNMENT, net_ctx->_return_code)
    net_ctx->_weights[idx] = weights[idx];
  }_stai_network_check(net_ctx);
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_set_inputs(
  stai_network* network,
  const stai_ptr* inputs,
  const stai_size n_inputs)
{
  const uintptr_t _inputs_alignment[] = STAI_NETWORK_IN_ALIGNMENTS;
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !inputs,
                  STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, n_inputs!=STAI_NETWORK_IN_NUM,
                  STAI_ERROR_NETWORK_INVALID_IN_NUM, net_ctx->_return_code)

  for (stai_size idx=0; inputs && idx<STAI_NETWORK_IN_NUM; idx++) {
    STAI_PRINT("  input[%d]: %p\n", idx, inputs[idx])
    _STAI_SET_ERROR(net_ctx, inputs[idx]==NULL,
                    STAI_ERROR_NETWORK_INVALID_IN_PTR, net_ctx->_return_code)
    _STAI_SET_ERROR(net_ctx, ((uintptr_t)inputs[idx]) & (_inputs_alignment[idx]-1),
                    STAI_ERROR_INVALID_BUFFER_ALIGNMENT, net_ctx->_return_code)
    net_ctx->_inputs[idx] = inputs[idx];
  }

  _stai_network_check(net_ctx);
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_set_outputs(
  stai_network* network,
  const stai_ptr* outputs,
  const stai_size n_outputs)
{
  const uintptr_t _outputs_alignment[] = STAI_NETWORK_OUT_ALIGNMENTS;
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !outputs,
                  STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, n_outputs!=STAI_NETWORK_OUT_NUM,
                  STAI_ERROR_NETWORK_INVALID_OUT_NUM, net_ctx->_return_code)

  for (stai_size idx=0; outputs && idx<n_outputs; idx++) {
    STAI_PRINT("  output[%d]: %p\n", idx, outputs[idx])
    _STAI_SET_ERROR(net_ctx, outputs[idx]==NULL,
                    STAI_ERROR_NETWORK_INVALID_OUT_PTR, net_ctx->_return_code)
    _STAI_SET_ERROR(net_ctx, ((uintptr_t)outputs[idx]) & (_outputs_alignment[idx]-1),
                    STAI_ERROR_INVALID_BUFFER_ALIGNMENT, net_ctx->_return_code)
    net_ctx->_outputs[idx] = outputs[idx];
  }

  _stai_network_check(net_ctx);
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_network_set_states(
  stai_network* network,
  const stai_ptr* states,
  const stai_size n_states)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  STAI_UNUSED(states)
  STAI_UNUSED(n_states)
_stai_network_check(net_ctx);
  return net_ctx->_return_code;
}

STAI_API_ENTRY
stai_return_code stai_network_set_callback(
  stai_network* network, const stai_event_cb cb, void* cb_cookie)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  STAI_PRINT("  set_callback %p cb %p cookie %p\n", net_ctx, cb, cb_cookie)
  // _STAI_SET_ERROR(net_ctx, cb==NULL, STAI_ERROR_NETWORK_INVALID_CALLBACK, net_ctx->_return_code)
  net_ctx->_callback = cb;
  net_ctx->_callback_cookie = cb_cookie;
  return net_ctx->_return_code;
}

#undef _STAI_SET_ERROR
#undef _STAI_CONTEXT_ALIGNMENT
#undef _STAI_CONTEXT_ACQUIRE
#undef _STAI_NETWORK_EVENT_NODE_START_CB
#undef _STAI_NETWORK_EVENT_NODE_STOP_CB
#undef _STAI_NETWORK_MODEL_SIGNATURE
#undef _STAI_NETWORK_DATETIME
#undef _STAI_NETWORK_COMPILE_DATETIME

