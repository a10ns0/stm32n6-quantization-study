7010cb48: mov.w         r4, #0
7010cb4c: vldrb.u8      q0, [r3], #16
7010cb50: vaddva.s8     r4, q0
7010cb54: vldrb.u8      q1, [r1], #16
7010cb58: vmlava.s8     r2, q0, q1
7010cb5c: vldrb.u8      q2, [r0], #16
7010cb60: vmlava.s8     r8, q0, q2
7010cb64: vldrb.u8      q3, [r7], #16
7010cb68: vmlava.s8     r6, q0, q3
7010cb6c: vldrb.u8      q4, [r5], #16
7010cb70: vmlava.s8     ip, q0, q4
7010cb74: letp          lr, 7010cb4c <st_sssa8_ch_nn_mat_mult_nt_t_s8+0xc4>
