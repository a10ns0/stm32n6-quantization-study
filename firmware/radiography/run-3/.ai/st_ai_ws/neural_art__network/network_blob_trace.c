#include "ll_aton_NN_interface.h"
#include "ll_aton.h"
#include "ll_aton_ec_trace.h"

#if 0
// Workaround: the tracer does not know the target at this moment
// and cannot call the functions since are used in static code
#define ATON_LIB_PHYSICAL_TO_VIRTUAL_ADDR(address) LL_Address_Physical2Virtual(address)
#define ATON_LIB_VIRTUAL_TO_PHYSICAL_ADDR(address) LL_Address_Virtual2Physical(address)
#else
#define ATON_LIB_PHYSICAL_TO_VIRTUAL_ADDR(address) (address)
#define ATON_LIB_VIRTUAL_TO_PHYSICAL_ADDR(address) (address)
#endif

#define _mem_pool_xSPI2_network 1

// MCU cache line size: 32 (bytes)
// NPU cache line size: 64 (bytes)
// MCU+NPU cache line size equal to 64 bytes (power of 2 not less than 8)
unsigned int cache_line_size = 64;

mpool_reloc_info_t mpool_reloc_info[] = {
  {"AXISRAM6", "_mem_pool_AXISRAM6_network", 0x34350000, 1, 0},
  {"AXISRAM5", "_mem_pool_AXISRAM5_network", 0x342e0000, 1, 0},
  {"AXISRAM4", "_mem_pool_AXISRAM4_network", 0x34270000, 1, 0},
  {"AXISRAM3", "_mem_pool_AXISRAM3_network", 0x34200000, 1, 0},
  {"AXISRAM2", "_mem_pool_AXISRAM2_network", 0x34100000, 1, 0},
  {"AXISRAM1", "_mem_pool_AXISRAM1_network", 0x34064000, 1, 0},
  {"AXIFLEXMEM", "_mem_pool_AXIFLEXMEM_network", 0x34000000, 1, 0},
  {"xSPI1", "_mem_pool_xSPI1_network", 0x90000000, 1, 0},
  {"xSPI2", "_mem_pool_xSPI2_network", 0x1, 0, 0},
  {"AXISRAM2_AXISRAM3_AXISRAM4_AXISRAM5_AXISRAM6", "_mem_pool_AXISRAM2_AXISRAM3_AXISRAM4_AXISRAM5_AXISRAM6_network", 0x34100000, 1, 0},
  {NULL, NULL, 0, 0, 0}
};


static const uint64_t __ec_params_memory_pool_base[39] = {
0xFF64C08000000000ULL,0x01C70000AD5AF20FULL,0xCA1A03330000C672ULL,0x0075CBD004000000ULL,0x159A0111D6320CDBULL,0xE1F21C15019EDDD4ULL,0x0148E2F71E2101AAULL,0x2805FFD5E2FD211FULL,
0xDC093231FE9FE126ULL,0x0000EB0020000000ULL,0x20000000EB002000ULL,0xEA8020000000EB00ULL,0x0000EA8020000000ULL,0x10000000EA002000ULL,0x0880100000000800ULL,0xFF16CE4C42ABFD55ULL,
0x08000000FA681FA8ULL,0x2580066600002060ULL,0x00003C0000000000ULL,0x0000000003001000ULL,0x0180100000003C80ULL,0x00003D0000000000ULL,0x000000003D000000ULL,0x3D80000000003D80ULL,
0xFF023E7CFF00694FULL,0xFF026FEEFF01DA72ULL,0xFEFF5023FF01D3B0ULL,0xFEFF5AF0FF027831ULL,0xFF001C44FEFF5018ULL,0xFF009408FEFECDF7ULL,0xFF021301FEFFE334ULL,0xFF0039DAFEFEBAAAULL,
0x40B8416839D53204ULL,0x17B33B86154C409DULL,0x3B361925359B1581ULL,0x37E3186744BA4845ULL,0x0807060543020100ULL,0xD8561514CC0B0A09ULL,0x0000000000000000ULL,
};

void trace_ec__ec_blob_network_1(void) {
  ec_trace_start_blob("_ec_blob_network_1");
  ec_trace_start_epoch(1);
  {
  }
  {
  }
  ec_trace_end_epoch(1);
  ec_trace_end_blob("_ec_blob_network_1");
}

void trace_ec__ec_blob_network_6(void) {
  ec_trace_start_blob("_ec_blob_network_6");
  ec_trace_start_epoch(6);
  {
    /* Unit= 22 [POOL_ACC_V2 0] */
    /* kind=MaxPool node=MaxPool_9 */
    static const LL_Poolacc_InitTypeDef MaxPool_9_init6 = {
      .operation = POOL_MAX,
      .avgnopad = 0,
      .inputX = 148,
      .inputY = 148,
      .outputX = 74,
      .outputY = 74,
      .poolWinX = 2,
      .poolWinY = 2,
      .strideX = 2,
      .strideY = 2,
      .topCrop = 0,
      .bottomCrop = 147,
      .leftCrop = 0,
      .rightCrop = 147,
      .topPad = 0,
      .bottomPad = 0,
      .leftPad = 0,
      .rightPad = 0,
      .batchSize = 6,
      .shift_f = 0,
      .shift_o = 0,
      .dualLine = 1,
      .rounding_f = 0,
      .saturation_f = 0,
      .round_mode_f = 0,
      .inbytes_f = 1,
      .outbytes_f = 1,
      .rounding_o = 1,
      .saturation_o = 1,
      .round_mode_o = 1,
      .relu_mode_o = 0,
      .outbytes_o = 1,
      .mulval = 0,
      .pad_val_en = 0,
      .pad_val = 0,
    };

    /* Unit=POOL_ACC_V2 */
    LL_Poolacc_Init(0, &MaxPool_9_init6);


    /* Dma inputs units to cycle: */
    /* Unit= 2 [STREAM_ENG_V2 2] */
    /* Emit conf for STREAM_ENG_V2 node=MaxPool_9 input ports=0 range=1[0,131424] */

    static const LL_Streng_TensorInitTypeDef MaxPool_9_dma_init_in_0_6 = {
      /* from memory with batch=6 */
      .dir = 0,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Dequantize_8_out_0 */
      .offset_start = 0,
      .offset_end = 131424,
      .offset_limit = 131488,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 131424,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(2, &MaxPool_9_dma_init_in_0_6, 1);


    /* Dma input bandwidth from memory pools: */
    /* npuRAM5 -> 131424 */

    /* Dma output units from cycle: */
    /* Unit= 9 [STREAM_ENG_V2 9] */
    /* Emit conf for STREAM_ENG_V2 node=MaxPool_9 output ports=0 range=1[131424,164280] */

    static const LL_Streng_TensorInitTypeDef MaxPool_9_dma_init_out_0_6 = {
      /* to memory with batch=6 */
      .dir = 1,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* MaxPool_9_out_0 */
      .offset_start = 131424,
      .offset_end = 164280,
      .offset_limit = 164344,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 32856,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(9, &MaxPool_9_dma_init_out_0_6, 1);


    /* Dma output bandwidth to memory pools: */
    /* npuRAM5 <- 32856 */

    static const LL_Switch_InitTypeDef STREAM_SWITCH_0_init_in_6[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, POOL, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 2, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_9 IN: in unit=POOL_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 2 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 9, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, POOL, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_9 OUT: in unit=STREAM_ENG_V2 9 in port=0 out unit=POOL_ACC_V2 0 out port=0 */
    };


    /* epoch=6 */
    LL_Switch_Init(STREAM_SWITCH_0_init_in_6, 2);

    static const LL_ATON_EnableUnits_InitTypeDef Enable_epoch_6_all_units[] = {
      { {STRENG, 9} }, /* STREAM_ENG_V2 */
      { {POOL, 0} }, /* POOL_ACC_V2 */
      { {STRENG, 2} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_EnableUnits_Init(Enable_epoch_6_all_units, 3);

  }

  ec_trace_wait_epoch_end(0x200);

  {
    static const LL_Switch_DeinitTypeDef STREAM_SWITCH_0_deinit_in_6[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, POOL, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 2, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_9 IN: in unit=POOL_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 2 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 9, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, POOL, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_9 OUT: in unit=STREAM_ENG_V2 9 in port=0 out unit=POOL_ACC_V2 0 out port=0 */
    };


    /* epoch=6 */
    LL_Switch_Deinit(STREAM_SWITCH_0_deinit_in_6, 2);

    static const LL_ATON_DisableUnits_InitTypeDef Disable_epoch_6_all_units[] = {
      { {STRENG, 9} }, /* STREAM_ENG_V2 */
      { {POOL, 0} }, /* POOL_ACC_V2 */
      { {STRENG, 2} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_DisableUnits_Init(Disable_epoch_6_all_units, 3);

  }
  ec_trace_end_epoch(6);
  ec_trace_end_blob("_ec_blob_network_6");
}

void trace_ec__ec_blob_network_11(void) {
  ec_trace_start_blob("_ec_blob_network_11");
  ec_trace_start_epoch(11);
  {
    /* Unit= 23 [POOL_ACC_V2 1] */
    /* kind=MaxPool node=MaxPool_16 */
    static const LL_Poolacc_InitTypeDef MaxPool_16_init11 = {
      .operation = POOL_MAX,
      .avgnopad = 0,
      .inputX = 72,
      .inputY = 72,
      .outputX = 36,
      .outputY = 36,
      .poolWinX = 2,
      .poolWinY = 2,
      .strideX = 2,
      .strideY = 2,
      .topCrop = 0,
      .bottomCrop = 71,
      .leftCrop = 0,
      .rightCrop = 71,
      .topPad = 0,
      .bottomPad = 0,
      .leftPad = 0,
      .rightPad = 0,
      .batchSize = 6,
      .shift_f = 0,
      .shift_o = 0,
      .dualLine = 1,
      .rounding_f = 0,
      .saturation_f = 0,
      .round_mode_f = 0,
      .inbytes_f = 1,
      .outbytes_f = 1,
      .rounding_o = 1,
      .saturation_o = 1,
      .round_mode_o = 1,
      .relu_mode_o = 0,
      .outbytes_o = 1,
      .mulval = 0,
      .pad_val_en = 0,
      .pad_val = 0,
    };

    /* Unit=POOL_ACC_V2 */
    LL_Poolacc_Init(1, &MaxPool_16_init11);


    /* Dma inputs units to cycle: */
    /* Unit= 2 [STREAM_ENG_V2 2] */
    /* Emit conf for STREAM_ENG_V2 node=MaxPool_16 input ports=0 range=1[0,31104] */

    static const LL_Streng_TensorInitTypeDef MaxPool_16_dma_init_in_0_11 = {
      /* from memory with batch=6 */
      .dir = 0,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Dequantize_15_out_0 */
      .offset_start = 0,
      .offset_end = 31104,
      .offset_limit = 31168,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 31104,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(2, &MaxPool_16_dma_init_in_0_11, 1);


    /* Dma input bandwidth from memory pools: */
    /* npuRAM5 -> 31104 */

    /* Dma output units from cycle: */
    /* Unit= 6 [STREAM_ENG_V2 6] */
    /* Emit conf for STREAM_ENG_V2 node=MaxPool_16 output ports=0 range=1[31104,38880] */

    static const LL_Streng_TensorInitTypeDef MaxPool_16_dma_init_out_0_11 = {
      /* to memory with batch=6 */
      .dir = 1,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* MaxPool_16_out_0 */
      .offset_start = 31104,
      .offset_end = 38880,
      .offset_limit = 38944,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 7776,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(6, &MaxPool_16_dma_init_out_0_11, 1);


    /* Dma output bandwidth to memory pools: */
    /* npuRAM5 <- 7776 */

    static const LL_Switch_InitTypeDef STREAM_SWITCH_0_init_in_11[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, POOL, 1, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 2, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_16 IN: in unit=POOL_ACC_V2 1 in port=0 out unit=STREAM_ENG_V2 2 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 6, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, POOL, 1, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_16 OUT: in unit=STREAM_ENG_V2 6 in port=0 out unit=POOL_ACC_V2 1 out port=0 */
    };


    /* epoch=11 */
    LL_Switch_Init(STREAM_SWITCH_0_init_in_11, 2);

    static const LL_ATON_EnableUnits_InitTypeDef Enable_epoch_11_all_units[] = {
      { {STRENG, 6} }, /* STREAM_ENG_V2 */
      { {POOL, 1} }, /* POOL_ACC_V2 */
      { {STRENG, 2} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_EnableUnits_Init(Enable_epoch_11_all_units, 3);

  }

  ec_trace_wait_epoch_end(0x40);

  {
    static const LL_Switch_DeinitTypeDef STREAM_SWITCH_0_deinit_in_11[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, POOL, 1, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 2, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_16 IN: in unit=POOL_ACC_V2 1 in port=0 out unit=STREAM_ENG_V2 2 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 6, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, POOL, 1, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_16 OUT: in unit=STREAM_ENG_V2 6 in port=0 out unit=POOL_ACC_V2 1 out port=0 */
    };


    /* epoch=11 */
    LL_Switch_Deinit(STREAM_SWITCH_0_deinit_in_11, 2);

    static const LL_ATON_DisableUnits_InitTypeDef Disable_epoch_11_all_units[] = {
      { {STRENG, 6} }, /* STREAM_ENG_V2 */
      { {POOL, 1} }, /* POOL_ACC_V2 */
      { {STRENG, 2} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_DisableUnits_Init(Disable_epoch_11_all_units, 3);

  }
  ec_trace_end_epoch(11);
  ec_trace_end_blob("_ec_blob_network_11");
}

void trace_ec__ec_blob_network_16(void) {
  ec_trace_start_blob("_ec_blob_network_16");
  ec_trace_start_epoch(16);
  {
    /* Unit= 22 [POOL_ACC_V2 0] */
    /* kind=MaxPool node=MaxPool_23 */
    static const LL_Poolacc_InitTypeDef MaxPool_23_init16 = {
      .operation = POOL_MAX,
      .avgnopad = 0,
      .inputX = 34,
      .inputY = 34,
      .outputX = 17,
      .outputY = 17,
      .poolWinX = 2,
      .poolWinY = 2,
      .strideX = 2,
      .strideY = 2,
      .topCrop = 0,
      .bottomCrop = 33,
      .leftCrop = 0,
      .rightCrop = 33,
      .topPad = 0,
      .bottomPad = 0,
      .leftPad = 0,
      .rightPad = 0,
      .batchSize = 6,
      .shift_f = 0,
      .shift_o = 0,
      .dualLine = 1,
      .rounding_f = 0,
      .saturation_f = 0,
      .round_mode_f = 0,
      .inbytes_f = 1,
      .outbytes_f = 1,
      .rounding_o = 1,
      .saturation_o = 1,
      .round_mode_o = 1,
      .relu_mode_o = 0,
      .outbytes_o = 1,
      .mulval = 0,
      .pad_val_en = 0,
      .pad_val = 0,
    };

    /* Unit=POOL_ACC_V2 */
    LL_Poolacc_Init(0, &MaxPool_23_init16);


    /* Dma inputs units to cycle: */
    /* Unit= 3 [STREAM_ENG_V2 3] */
    /* Emit conf for STREAM_ENG_V2 node=MaxPool_23 input ports=0 range=1[0,6936] */

    static const LL_Streng_TensorInitTypeDef MaxPool_23_dma_init_in_0_16 = {
      /* from memory with batch=6 */
      .dir = 0,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Dequantize_22_out_0 */
      .offset_start = 0,
      .offset_end = 6936,
      .offset_limit = 7000,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 6936,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(3, &MaxPool_23_dma_init_in_0_16, 1);


    /* Dma input bandwidth from memory pools: */
    /* npuRAM5 -> 6936 */

    /* Dma output units from cycle: */
    /* Unit= 8 [STREAM_ENG_V2 8] */
    /* Emit conf for STREAM_ENG_V2 node=MaxPool_23 output ports=0 range=1[6944,8678] */

    static const LL_Streng_TensorInitTypeDef MaxPool_23_dma_init_out_0_16 = {
      /* to memory with batch=6 */
      .dir = 1,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* MaxPool_23_out_0 */
      .offset_start = 6944,
      .offset_end = 8678,
      .offset_limit = 8744,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 1734,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(8, &MaxPool_23_dma_init_out_0_16, 1);


    /* Dma output bandwidth to memory pools: */
    /* npuRAM5 <- 1734 */

    static const LL_Switch_InitTypeDef STREAM_SWITCH_0_init_in_16[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, POOL, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 3, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_23 IN: in unit=POOL_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 3 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 8, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, POOL, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_23 OUT: in unit=STREAM_ENG_V2 8 in port=0 out unit=POOL_ACC_V2 0 out port=0 */
    };


    /* epoch=16 */
    LL_Switch_Init(STREAM_SWITCH_0_init_in_16, 2);

    static const LL_ATON_EnableUnits_InitTypeDef Enable_epoch_16_all_units[] = {
      { {STRENG, 8} }, /* STREAM_ENG_V2 */
      { {POOL, 0} }, /* POOL_ACC_V2 */
      { {STRENG, 3} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_EnableUnits_Init(Enable_epoch_16_all_units, 3);

  }

  ec_trace_wait_epoch_end(0x100);

  {
    static const LL_Switch_DeinitTypeDef STREAM_SWITCH_0_deinit_in_16[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, POOL, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 3, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_23 IN: in unit=POOL_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 3 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 8, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, POOL, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* MaxPool_23 OUT: in unit=STREAM_ENG_V2 8 in port=0 out unit=POOL_ACC_V2 0 out port=0 */
    };


    /* epoch=16 */
    LL_Switch_Deinit(STREAM_SWITCH_0_deinit_in_16, 2);

    static const LL_ATON_DisableUnits_InitTypeDef Disable_epoch_16_all_units[] = {
      { {STRENG, 8} }, /* STREAM_ENG_V2 */
      { {POOL, 0} }, /* POOL_ACC_V2 */
      { {STRENG, 3} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_DisableUnits_Init(Disable_epoch_16_all_units, 3);

  }
  ec_trace_end_epoch(16);
  ec_trace_end_blob("_ec_blob_network_16");
}

void trace_ec__ec_blob_network_21(void) {
  ec_trace_start_blob("_ec_blob_network_21");
  ec_trace_start_epoch(21);
  {
  }
  {
  }
  ec_trace_end_epoch(21);
  ec_trace_start_epoch(22);
  {
    /* Unit= 18 [ARITH_ACC_V2 0] */
    /* kind=Sub node=Gemm_32_conv_4_suboff_13 */
    static const LL_Arithacc_InitTypeDef Gemm_32_conv_4_suboff_13_init22 = {
      .rounding_x = 0,
      .saturation_x = 0,
      .round_mode_x = 0,
      .inbytes_x = 1,
      .outbytes_x = 1,
      .shift_x = 0,
      .rounding_y = 0,
      .saturation_y = 0,
      .round_mode_y = 0,
      .inbytes_y = 1,
      .outbytes_y = 1,
      .combinebc = 1,
      .clipout = 0,
      .shift_y = 0,
      .rounding_o = 0,
      .saturation_o = 0,
      .round_mode_o = 1,
      .relu_mode_o = 0,
      .outbytes_o = 1,
      .shift_o = 14,
      .scalar = 1,
      .dualinput = 0,
      .operation = ARITH_AFFINE,
      .bcast = ARITH_BCAST_SCALAR,
      .Ax_shift = 0,
      .By_shift = 0,
      .C_shift = 0,
      .fWidth = 1,
      .fHeight = 1,
      .fChannels = 1350,
      .batchDepth = 1,
      .clipmin = 0,
      .clipmax = 0,
      .A_scalar = 16384,
      .B_scalar = 32,
      .C_scalar = (short)0,
      .A_vector = {0},
      .B_vector = {0},
      .C_vector = {0},
      .vec_precision = {8, 8, 8},
    };

    /* Unit=ARITH_ACC_V2 */
    LL_Arithacc_Init(0, &Gemm_32_conv_4_suboff_13_init22);


    /* Dma inputs units to cycle: */
    /* Unit= 3 [STREAM_ENG_V2 3] */
    /* Emit conf for STREAM_ENG_V2 node=Gemm_32_conv_4_suboff_13 input ports=0 range=1[0,1350] */

    static const LL_Streng_TensorInitTypeDef Gemm_32_conv_4_suboff_13_dma_init_in_0_22 = {
      /* from memory with batch=1 */
      .dir = 0,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Gemm_32_reshape_x_2 */
      .offset_start = 0,
      .offset_end = 1,
      .offset_limit = 1416,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 1,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1350,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(3, &Gemm_32_conv_4_suboff_13_dma_init_in_0_22, 1);


    /* Dma input bandwidth from memory pools: */
    /* npuRAM5 -> 1350 */

    /* Dma output units from cycle: */
    /* Unit= 9 [STREAM_ENG_V2 9] */
    /* Emit conf for STREAM_ENG_V2 node=Gemm_32_conv_4_suboff_13 output ports=0 range=1[0,1350] */

    static const LL_Streng_TensorInitTypeDef Gemm_32_conv_4_suboff_13_dma_init_out_0_22 = {
      /* to memory with batch=1 */
      .dir = 1,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 1,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Gemm_32_conv_4_zero_off_out_14 */
      .offset_start = 0,
      .offset_end = 1,
      .offset_limit = 1416,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 1,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1350,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(9, &Gemm_32_conv_4_suboff_13_dma_init_out_0_22, 1);


    /* Dma output bandwidth to memory pools: */
    /* npuRAM5 <- 1350 */

    static const LL_Switch_InitTypeDef STREAM_SWITCH_0_init_in_22[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, ARITH, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 3, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_suboff_13 IN: in unit=ARITH_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 3 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 9, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, ARITH, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_suboff_13 OUT: in unit=STREAM_ENG_V2 9 in port=0 out unit=ARITH_ACC_V2 0 out port=0 */
    };


    /* epoch=22 */
    LL_Switch_Init(STREAM_SWITCH_0_init_in_22, 2);

    static const LL_ATON_EnableUnits_InitTypeDef Enable_epoch_22_all_units[] = {
      { {STRENG, 9} }, /* STREAM_ENG_V2 */
      { {ARITH, 0} }, /* ARITH_ACC_V2 */
      { {STRENG, 3} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_EnableUnits_Init(Enable_epoch_22_all_units, 3);

  }

  ec_trace_wait_epoch_end(0x200);

  {
    static const LL_Switch_DeinitTypeDef STREAM_SWITCH_0_deinit_in_22[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, ARITH, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 3, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_suboff_13 IN: in unit=ARITH_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 3 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 9, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, ARITH, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_suboff_13 OUT: in unit=STREAM_ENG_V2 9 in port=0 out unit=ARITH_ACC_V2 0 out port=0 */
    };


    /* epoch=22 */
    LL_Switch_Deinit(STREAM_SWITCH_0_deinit_in_22, 2);

    static const LL_ATON_DisableUnits_InitTypeDef Disable_epoch_22_all_units[] = {
      { {STRENG, 9} }, /* STREAM_ENG_V2 */
      { {ARITH, 0} }, /* ARITH_ACC_V2 */
      { {STRENG, 3} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_DisableUnits_Init(Disable_epoch_22_all_units, 3);

  }
  ec_trace_end_epoch(22);
  ec_trace_start_epoch(23);
  {
    /* Unit= 10 [CONV_ACC_V2 0] */
    /* kind=Conv node=Gemm_32_conv_4 */
    static const LL_Convacc_InitTypeDef Gemm_32_conv_4_init23 = {
      .simd = 1,
      .fsub = 0,
      .vshift = 0,
      .accumulate = 1,
      .accumulate_gen_first = 1,
      .accumulate_first = 1,
      .afilt_mode = AFILT_MODE_FRAMEZERO,
      .afilt_tot = 5,
      .afilt_first = 1,
      .afilt_last = 4,
      .kfilt_tot = 48,
      .kfilt_first = 0,
      .kfilt_last = 15,
      .rounding_f = 0,
      .saturation_f = 0,
      .round_mode_f = 0,
      .f_unsigned = 1,
      .k_unsigned = 0,
      .deepmode = 1,
      .dss2mode = 0,
      .kseten = 3,
      .zfbias = 0,
      .inbytes_f = 1,
      .shift_f = 0,
      .shift_a = 8,
      .rounding_o = 1,
      .saturation_o = 1,
      .round_mode_o = 1,
      .relu_mode_o = 0,
      .outbytes_o = 3,
      .shift_o = 0,
      .raw_o = 1,
      .fWidth = 1,
      .fHeight = 1,
      .kernelWidth = 1,
      .kernelHeight = 1,
      .nKernels = 16,
      .batchDepth = 90,
      .hstride = 1,
      .vstride = 1,
      .left_padding = 0,
      .right_padding = 0,
      .top_padding = 0,
      .bot_padding = 0,
      .left_crop = 0,
      .right_crop = 0,
      .top_crop = 0,
      .bot_crop = 0,
    };

    /* Unit=CONV_ACC_V2 */
    LL_Convacc_Init(0, &Gemm_32_conv_4_init23);


    /* Unit= 11 [CONV_ACC_V2 1] */
    /* kind=Conv node=Gemm_32_conv_4_ca_pipe_1 */
    static const LL_Convacc_InitTypeDef Gemm_32_conv_4_ca_pipe_1_init23 = {
      .simd = 1,
      .fsub = 0,
      .vshift = 0,
      .accumulate = 1,
      .accumulate_first = 1,
      .kfilt_tot = 48,
      .kfilt_first = 16,
      .kfilt_last = 31,
      .rounding_f = 0,
      .saturation_f = 0,
      .round_mode_f = 0,
      .f_unsigned = 1,
      .k_unsigned = 0,
      .deepmode = 1,
      .dss2mode = 0,
      .kseten = 3,
      .zfbias = 0,
      .inbytes_f = 1,
      .shift_f = 0,
      .shift_a = 0,
      .rounding_o = 1,
      .saturation_o = 1,
      .round_mode_o = 1,
      .relu_mode_o = 0,
      .outbytes_o = 3,
      .shift_o = 0,
      .raw_o = 1,
      .fWidth = 1,
      .fHeight = 1,
      .kernelWidth = 1,
      .kernelHeight = 1,
      .nKernels = 16,
      .batchDepth = 90,
      .hstride = 1,
      .vstride = 1,
      .left_padding = 0,
      .right_padding = 0,
      .top_padding = 0,
      .bot_padding = 0,
      .left_crop = 0,
      .right_crop = 0,
      .top_crop = 0,
      .bot_crop = 0,
    };

    /* Unit=CONV_ACC_V2 */
    LL_Convacc_Init(1, &Gemm_32_conv_4_ca_pipe_1_init23);


    /* Unit= 12 [CONV_ACC_V2 2] */
    /* kind=Conv node=Gemm_32_conv_4_ca_pipe_2 */
    static const LL_Convacc_InitTypeDef Gemm_32_conv_4_ca_pipe_2_init23 = {
      .simd = 1,
      .fsub = 0,
      .vshift = 0,
      .accumulate = 1,
      .accumulate_first = 1,
      .kfilt_tot = 48,
      .kfilt_first = 32,
      .kfilt_last = 47,
      .rounding_f = 0,
      .saturation_f = 0,
      .round_mode_f = 0,
      .f_unsigned = 1,
      .k_unsigned = 0,
      .deepmode = 1,
      .dss2mode = 0,
      .kseten = 3,
      .zfbias = 0,
      .inbytes_f = 1,
      .shift_f = 0,
      .shift_a = 0,
      .rounding_o = 1,
      .saturation_o = 1,
      .round_mode_o = 1,
      .relu_mode_o = 0,
      .outbytes_o = 2,
      .shift_o = 8,
      .raw_o = 0,
      .fWidth = 1,
      .fHeight = 1,
      .kernelWidth = 1,
      .kernelHeight = 1,
      .nKernels = 16,
      .batchDepth = 90,
      .hstride = 1,
      .vstride = 1,
      .left_padding = 0,
      .right_padding = 0,
      .top_padding = 0,
      .bot_padding = 0,
      .left_crop = 0,
      .right_crop = 0,
      .top_crop = 0,
      .bot_crop = 0,
    };

    /* Unit=CONV_ACC_V2 */
    LL_Convacc_Init(2, &Gemm_32_conv_4_ca_pipe_2_init23);


    /* Unit= 19 [ARITH_ACC_V2 1] */
    /* kind=Add node=Gemm_32_conv_4_off_bias_19 */
    static const LL_Arithacc_InitTypeDef Gemm_32_conv_4_off_bias_19_init23 = {
      .rounding_x = 0,
      .saturation_x = 0,
      .round_mode_x = 0,
      .inbytes_x = 2,
      .outbytes_x = 2,
      .shift_x = 0,
      .rounding_y = 0,
      .saturation_y = 0,
      .round_mode_y = 0,
      .inbytes_y = 2,
      .outbytes_y = 2,
      .combinebc = 1,
      .clipout = 0,
      .shift_y = 0,
      .rounding_o = 1,
      .saturation_o = 1,
      .round_mode_o = 1,
      .relu_mode_o = 0,
      .outbytes_o = 1,
      .shift_o = 17,
      .scalar = 0,
      .dualinput = 0,
      .operation = ARITH_AFFINE,
      .bcast = ARITH_BCAST_CHAN,
      .Ax_shift = 0,
      .By_shift = 0,
      .C_shift = 0,
      .fWidth = 1,
      .fHeight = 1,
      .fChannels = 16,
      .batchDepth = 16,
      .clipmin = 0,
      .clipmax = 0,
      .A_scalar = 0,
      .B_scalar = 0,
      .C_scalar = (short)0,
      .A_vector = {((unsigned char *)((((uintptr_t)(__ec_params_memory_pool_base)) + 256))) /* Equivalent hex offset = 0x100 */},
      .B_vector = {0},
      .C_vector = {((unsigned char *)((((uintptr_t)(__ec_params_memory_pool_base)) + 192))) /* Equivalent hex offset = 0xc0 */},
      .vec_precision = {16, 16, 32},
    };

    /* Unit=ARITH_ACC_V2 */
    LL_Arithacc_Init(1, &Gemm_32_conv_4_off_bias_19_init23);


    /* Dma inputs units to cycle: */
    /* Unit= 8 [STREAM_ENG_V2 8] */
    /* Emit conf for STREAM_ENG_V2 node=Gemm_32_conv_4 input ports=0 range=1[0,1350] */

    static const LL_Streng_TensorInitTypeDef Gemm_32_conv_4_dma_init_in_0_23 = {
      .dir = 0,
      .raw_out = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 1,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Gemm_32_conv_4_zero_off_out_14 */
      .offset_start = 0,
      .offset_limit = 1416,
      .frame_count = 0,
      .fwidth = 1,
      .fheight = 1,
      .batch_depth = 30,
      .batch_offset = 1350,
      .frame_offset = 270,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 5,
      .frame_tot_cnt = 5,
      .nbits_in = 24,
      .nbits_out = 24,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(8, &Gemm_32_conv_4_dma_init_in_0_23, 1);

    /* Unit= 4 [STREAM_ENG_V2 4] */
    /* Emit conf for STREAM_ENG_V2 node=Gemm_32_conv_4 input ports=1 range=8[0,21600] */

    static const LL_Streng_TensorInitTypeDef Gemm_32_conv_4_dma_init_in_1_23 = {
      /* 16x1x1x1350(8 bits) */
      .dir = 0,
      .raw = 1,
      .continuous = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .cacheable = 1,
      .cache_allocate = 0,
      .addr_base = {(unsigned char *)ATON_LIB_VIRTUAL_TO_PHYSICAL_ADDR((uintptr_t)_mem_pool_xSPI2_network)}, /* Gemm_32_weights_transposed_3 */
      .offset_start = 0,
      .offset_end = 21600,
      .offset_limit = 21664,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 0,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 24,
      .nbits_out = 24,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(4, &Gemm_32_conv_4_dma_init_in_1_23, 1);

    /* Unit= 5 [STREAM_ENG_V2 5] */
    /* Emit conf for STREAM_ENG_V2 node=Gemm_32_conv_4_ca_pipe_1 input ports=0 range=1[0,1350] */

    static const LL_Streng_TensorInitTypeDef Gemm_32_conv_4_ca_pipe_1_dma_init_in_0_23 = {
      .dir = 0,
      .raw_out = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 1,
      .addr_base = {(unsigned char *)ATON_LIB_PHYSICAL_TO_VIRTUAL_ADDR(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Gemm_32_conv_4_zero_off_out_14_copy_in_5 ca pipe offset=1 */
      .offset_start = 90,
      .offset_limit = 1416,
      .frame_count = 0,
      .fwidth = 1,
      .fheight = 1,
      .batch_depth = 30,
      .batch_offset = 1350,
      .frame_offset = 270,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 5,
      .frame_tot_cnt = 5,
      .nbits_in = 24,
      .nbits_out = 24,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(5, &Gemm_32_conv_4_ca_pipe_1_dma_init_in_0_23, 1);

    /* Unit= 9 [STREAM_ENG_V2 9] */
    /* Emit conf for STREAM_ENG_V2 node=Gemm_32_conv_4_ca_pipe_2 input ports=0 range=1[0,1350] */

    static const LL_Streng_TensorInitTypeDef Gemm_32_conv_4_ca_pipe_2_dma_init_in_0_23 = {
      .dir = 0,
      .raw_out = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 1,
      .addr_base = {(unsigned char *)ATON_LIB_PHYSICAL_TO_VIRTUAL_ADDR(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Gemm_32_conv_4_zero_off_out_14_copy_in_6 ca pipe offset=2 */
      .offset_start = 180,
      .offset_limit = 1416,
      .frame_count = 0,
      .fwidth = 1,
      .fheight = 1,
      .batch_depth = 30,
      .batch_offset = 1350,
      .frame_offset = 270,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 5,
      .frame_tot_cnt = 5,
      .nbits_in = 24,
      .nbits_out = 24,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(9, &Gemm_32_conv_4_ca_pipe_2_dma_init_in_0_23, 1);


    /* Dma input bandwidth from memory pools: */
    /* npuRAM5 -> 1350 */
    /* octoFlash -> 21600 */

    /* Dma output units from cycle: */
    /* Unit= 3 [STREAM_ENG_V2 3] */
    /* Emit conf for STREAM_ENG_V2 node=Gemm_32_conv_4_off_bias_19 output ports=0 range=1[1360,1376] */

    static const LL_Streng_TensorInitTypeDef Gemm_32_conv_4_off_bias_19_dma_init_out_0_23 = {
      /* to memory with batch=16 */
      .dir = 1,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Gemm_32_conv_4_off_bias_out_20 */
      .offset_start = 1360,
      .offset_end = 1376,
      .offset_limit = 1440,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 16,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(3, &Gemm_32_conv_4_off_bias_19_dma_init_out_0_23, 1);


    /* Dma output bandwidth to memory pools: */
    /* npuRAM5 <- 16 */

    static const LL_Switch_InitTypeDef STREAM_SWITCH_0_init_in_23[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 8, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4 IN: in unit=CONV_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 8 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 0, 1), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 4, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4 IN: in unit=CONV_ACC_V2 0 in port=1 out unit=STREAM_ENG_V2 4 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 0, 2), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 2, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4 IN: in unit=CONV_ACC_V2 0 in port=2 out unit=CONV_ACC_V2 2 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 1, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 5, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_1 IN: in unit=CONV_ACC_V2 1 in port=0 out unit=STREAM_ENG_V2 5 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 1, 1), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 4, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_1 IN: in unit=CONV_ACC_V2 1 in port=1 out unit=STREAM_ENG_V2 4 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 1, 2), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_1 IN: in unit=CONV_ACC_V2 1 in port=2 out unit=CONV_ACC_V2 0 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 2, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 9, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_2 IN: in unit=CONV_ACC_V2 2 in port=0 out unit=STREAM_ENG_V2 9 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 2, 1), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 4, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_2 IN: in unit=CONV_ACC_V2 2 in port=1 out unit=STREAM_ENG_V2 4 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 2, 2), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 1, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_2 IN: in unit=CONV_ACC_V2 2 in port=2 out unit=CONV_ACC_V2 1 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, ARITH, 1, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 2, 0), LL_Switch_Init_Context(0) = 0, LL_Switch_Init_Frames(0) = 4, LL_Switch_Init_Source(1) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 2, 0), LL_Switch_Init_Context(1) = 1, LL_Switch_Init_Frames(1) = 1, }, /* Gemm_32_conv_4_off_bias_19 IN: in unit=ARITH_ACC_V2 1 in port=0 out unit=CONV_ACC_V2 2 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 3, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, ARITH, 1, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_off_bias_19 OUT: in unit=STREAM_ENG_V2 3 in port=0 out unit=ARITH_ACC_V2 1 out port=0 */
    };


    /* epoch=23 */
    LL_Switch_Init(STREAM_SWITCH_0_init_in_23, 11);

    static const LL_ATON_EnableUnits_InitTypeDef Enable_epoch_23_all_units[] = {
      { {STRENG, 3} }, /* STREAM_ENG_V2 */
      { {CONVACC, 0} }, /* CONV_ACC_V2 */
      { {CONVACC, 1} }, /* CONV_ACC_V2 */
      { {CONVACC, 2} }, /* CONV_ACC_V2 */
      { {ARITH, 1} }, /* ARITH_ACC_V2 */
      { {STRENG, 4} }, /* STREAM_ENG_V2 */
      { {STRENG, 5} }, /* STREAM_ENG_V2 */
      { {STRENG, 8} }, /* STREAM_ENG_V2 */
      { {STRENG, 9} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_EnableUnits_Init(Enable_epoch_23_all_units, 9);

  }

  ec_trace_wait_epoch_end(0x8);

  {
    static const LL_Switch_DeinitTypeDef STREAM_SWITCH_0_deinit_in_23[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 8, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4 IN: in unit=CONV_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 8 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 0, 1), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 4, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4 IN: in unit=CONV_ACC_V2 0 in port=1 out unit=STREAM_ENG_V2 4 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 0, 2), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 2, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4 IN: in unit=CONV_ACC_V2 0 in port=2 out unit=CONV_ACC_V2 2 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 1, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 5, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_1 IN: in unit=CONV_ACC_V2 1 in port=0 out unit=STREAM_ENG_V2 5 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 1, 1), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 4, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_1 IN: in unit=CONV_ACC_V2 1 in port=1 out unit=STREAM_ENG_V2 4 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 1, 2), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_1 IN: in unit=CONV_ACC_V2 1 in port=2 out unit=CONV_ACC_V2 0 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 2, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 9, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_2 IN: in unit=CONV_ACC_V2 2 in port=0 out unit=STREAM_ENG_V2 9 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 2, 1), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 4, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_2 IN: in unit=CONV_ACC_V2 2 in port=1 out unit=STREAM_ENG_V2 4 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, CONVACC, 2, 2), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 1, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_ca_pipe_2 IN: in unit=CONV_ACC_V2 2 in port=2 out unit=CONV_ACC_V2 1 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, ARITH, 1, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 2, 0), LL_Switch_Init_Context(0) = 0, LL_Switch_Init_Frames(0) = 4, LL_Switch_Init_Source(1) = ATONN_SRCPORT(STRSWITCH, 0, CONVACC, 2, 0), LL_Switch_Init_Context(1) = 1, LL_Switch_Init_Frames(1) = 1, }, /* Gemm_32_conv_4_off_bias_19 IN: in unit=ARITH_ACC_V2 1 in port=0 out unit=CONV_ACC_V2 2 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 3, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, ARITH, 1, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Gemm_32_conv_4_off_bias_19 OUT: in unit=STREAM_ENG_V2 3 in port=0 out unit=ARITH_ACC_V2 1 out port=0 */
    };


    /* epoch=23 */
    LL_Switch_Deinit(STREAM_SWITCH_0_deinit_in_23, 11);

    static const LL_ATON_DisableUnits_InitTypeDef Disable_epoch_23_all_units[] = {
      { {STRENG, 3} }, /* STREAM_ENG_V2 */
      { {CONVACC, 0} }, /* CONV_ACC_V2 */
      { {CONVACC, 1} }, /* CONV_ACC_V2 */
      { {CONVACC, 2} }, /* CONV_ACC_V2 */
      { {ARITH, 1} }, /* ARITH_ACC_V2 */
      { {STRENG, 4} }, /* STREAM_ENG_V2 */
      { {STRENG, 5} }, /* STREAM_ENG_V2 */
      { {STRENG, 8} }, /* STREAM_ENG_V2 */
      { {STRENG, 9} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_DisableUnits_Init(Disable_epoch_23_all_units, 9);

  }
  ec_trace_end_epoch(23);
  ec_trace_end_blob("_ec_blob_network_21");
}

void trace_ec__ec_blob_network_25(void) {
  ec_trace_start_blob("_ec_blob_network_25");
  ec_trace_start_epoch(25);
  {
  }
  {
  }
  ec_trace_end_epoch(25);
  ec_trace_end_blob("_ec_blob_network_25");
}

void trace_ec__ec_blob_network_27(void) {
  ec_trace_start_blob("_ec_blob_network_27");
  ec_trace_start_epoch(27);
  {
  }
  {
  }
  ec_trace_end_epoch(27);
  ec_trace_end_blob("_ec_blob_network_27");
}

void trace_ec__ec_blob_network_30(void) {
  ec_trace_start_blob("_ec_blob_network_30");
  ec_trace_start_epoch(30);
  {
    /* Unit= 16 [ACTIV_ACC_V2 0] */
    /* kind=Sigmoid node=Sigmoid_41 */
    static const LL_Activacc_InitTypeDef Sigmoid_41_init30 = {
      .rounding_f = 0,
      .saturation_f = 0,
      .round_mode_f = 0,
      .inbytes_f = 1,
      .outbytes_f = 1,
      .rounding_o = 1,
      .saturation_o = 1,
      .round_mode_o = 1,
      .relu_mode_o = 0,
      .outbytes_o = 1,
      .signedop = 1,
      .shift_f = 0,
      .shift_o = 15,
      .parameter = 32640,
      .parameter_2 = 0,
      .ROM0_vector = {((unsigned char *)(ATON_LIB_VIRTUAL_TO_PHYSICAL_ADDR(((uintptr_t)(__ec_params_memory_pool_base)) + 288))) /* Equivalent hex offset = 0x120 */},
      .ROM1_vector = {((unsigned char *)(ATON_LIB_VIRTUAL_TO_PHYSICAL_ADDR(((uintptr_t)(__ec_params_memory_pool_base)) + 0))) /* Equivalent hex offset = 0x0 */},
      .ROM0_nbytes = 16,
      .ROM1_nbytes = 192,
      .shift_b = 3,
      .shift_c = 8,
      .shift_norm = 8,
      .bwidth = 4,
      .fsub = 0,
      .operation = ACTIV_FUNC,
    };

    /* Unit=ACTIV_ACC_V2 */
    LL_Activacc_Init(0, &Sigmoid_41_init30);


    /* Dma inputs units to cycle: */
    /* Unit= 1 [STREAM_ENG_V2 1] */
    /* Emit conf for STREAM_ENG_V2 node=Sigmoid_41 input ports=0 range=1[16,18] */

    static const LL_Streng_TensorInitTypeDef Sigmoid_41_dma_init_in_0_30 = {
      /* from memory with batch=1 */
      .dir = 0,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Dequantize_40_out_0 */
      .offset_start = 16,
      .offset_end = 18,
      .offset_limit = 88,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 2,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(1, &Sigmoid_41_dma_init_in_0_30, 1);


    /* Dma input bandwidth from memory pools: */
    /* npuRAM5 -> 2 */

    /* Dma output units from cycle: */
    /* Unit= 9 [STREAM_ENG_V2 9] */
    /* Emit conf for STREAM_ENG_V2 node=Sigmoid_41 output ports=0 range=1[0,2] */

    static const LL_Streng_TensorInitTypeDef Sigmoid_41_dma_init_out_0_30 = {
      /* to memory with batch=1 */
      .dir = 1,
      .raw = 1,
      .noblk = 0,
      .align_right = 0,
      .nbits_unsigned = 0,
      .addr_base = {(unsigned char *)(0x342e0000UL) /* Equivalent hex address = 0x342e0000UL */}, /* Quantize_42_out_0 */
      .offset_start = 0,
      .offset_end = 2,
      .offset_limit = 72,
      .frame_count = 0,
      .fwidth = 0,
      .fheight = 0,
      .batch_depth = 0,
      .batch_offset = 0,
      .frame_offset = 2,
      .line_offset = 0,
      .loop_offset = 0,
      .frame_loop_cnt = 0,
      .frame_tot_cnt = 1,
      .nbits_in = 8,
      .nbits_out = 8,
    };

    /* Unit=STREAM_ENG_V2 */
    LL_Streng_TensorInit(9, &Sigmoid_41_dma_init_out_0_30, 1);


    /* Dma output bandwidth to memory pools: */
    /* npuRAM5 <- 2 */

    static const LL_Switch_InitTypeDef STREAM_SWITCH_0_init_in_30[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, ACTIV, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 1, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Sigmoid_41 IN: in unit=ACTIV_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 1 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 9, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, ACTIV, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Sigmoid_41 OUT: in unit=STREAM_ENG_V2 9 in port=0 out unit=ACTIV_ACC_V2 0 out port=0 */
    };


    /* epoch=30 */
    LL_Switch_Init(STREAM_SWITCH_0_init_in_30, 2);

    static const LL_ATON_EnableUnits_InitTypeDef Enable_epoch_30_all_units[] = {
      { {STRENG, 9} }, /* STREAM_ENG_V2 */
      { {ACTIV, 0} }, /* ACTIV_ACC_V2 */
      { {STRENG, 1} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_EnableUnits_Init(Enable_epoch_30_all_units, 3);

  }

  ec_trace_wait_epoch_end(0x200);

  {
    static const LL_Switch_DeinitTypeDef STREAM_SWITCH_0_deinit_in_30[] = {
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, ACTIV, 0, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, STRENG, 1, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Sigmoid_41 IN: in unit=ACTIV_ACC_V2 0 in port=0 out unit=STREAM_ENG_V2 1 out port=0 */
      { LL_Switch_Init_Dest() = ATONN_DSTPORT(STRSWITCH, 0, STRENG, 9, 0), LL_Switch_Init_Source(0) = ATONN_SRCPORT(STRSWITCH, 0, ACTIV, 0, 0), LL_Switch_Init_Context(0) = 1, LL_Switch_Init_Frames(0) = 0, }, /* Sigmoid_41 OUT: in unit=STREAM_ENG_V2 9 in port=0 out unit=ACTIV_ACC_V2 0 out port=0 */
    };


    /* epoch=30 */
    LL_Switch_Deinit(STREAM_SWITCH_0_deinit_in_30, 2);

    static const LL_ATON_DisableUnits_InitTypeDef Disable_epoch_30_all_units[] = {
      { {STRENG, 9} }, /* STREAM_ENG_V2 */
      { {ACTIV, 0} }, /* ACTIV_ACC_V2 */
      { {STRENG, 1} }, /* STREAM_ENG_V2 */
    };


    LL_ATON_DisableUnits_Init(Disable_epoch_30_all_units, 3);

  }
  ec_trace_end_epoch(30);
  ec_trace_start_epoch(31);
  {
    /* Dma input bandwidth from memory pools: */
    /* npuRAM5 -> 0 */

  }
  {
  }
  ec_trace_end_epoch(31);
  ec_trace_end_blob("_ec_blob_network_30");
}


int main () {
  ec_trace_init("network_ecblobs.h", "network", false, 0, false);
  trace_ec__ec_blob_network_1();
  trace_ec__ec_blob_network_6();
  trace_ec__ec_blob_network_11();
  trace_ec__ec_blob_network_16();
  trace_ec__ec_blob_network_21();
  trace_ec__ec_blob_network_25();
  trace_ec__ec_blob_network_27();
  trace_ec__ec_blob_network_30();
  ec_trace_all_blobs_done();
}
