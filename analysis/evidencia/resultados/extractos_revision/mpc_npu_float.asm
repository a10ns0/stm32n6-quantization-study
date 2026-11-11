7010f52c: vldrw.u32     q3, [r5], #16
7010f530: vldrw.u32     q1, [r0], #16
7010f534: vmul.f32      q3, q3, q1
7010f538: vadd.f32      q2, q2, q3
7010f53c: le            lr, 7010f52c <forward_lite_conv2d_if32of32wf32_group+0x27c>
7010f540: vmov.32       r0, d4[1]
7010f544: vmov          s14, r0
7010f548: vmov.32       r0, d5[0]
7010f54c: vadd.f32      s14, s14, s8
