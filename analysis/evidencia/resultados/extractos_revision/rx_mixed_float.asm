7010a95e: vldrw.u32     q3, [r5], #16
7010a962: vldrw.u32     q1, [r0], #16
7010a966: vmul.f32      q3, q3, q1
7010a96a: vadd.f32      q2, q2, q3
7010a96e: le            lr, 7010a95e <forward_lite_conv2d_if32of32wf32+0x2ce>
7010a972: vmov.32       r0, d4[1]
7010a976: vmov          s15, r0
7010a97a: vmov.32       r0, d5[0]
7010a97e: vadd.f32      s15, s15, s8
