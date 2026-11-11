7010bcf0: vqsub.s32     q3, q3, r3
7010bcf4: vcvt.f32.s32  q3, q3
7010bcf8: vmul.f32      q3, q3, lr
7010bcfc: cmp           r2, r6
7010bcfe: vpst          
7010bd02: vstrwt.32     q3, [r1, #0]
7010bd06: add.w         r1, r1, #16
7010bd0a: beq.n         7010bd42 <lite_convert_is8of32+0x72>
