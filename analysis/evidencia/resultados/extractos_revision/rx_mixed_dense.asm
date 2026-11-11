7010bc5c: subs          r5, r7, r3
7010bc5e: subs          r2, r4, r3
7010bc60: vpst          
7010bc64: vldrbt.u8     q0, [r5, #0]
7010bc68: vmlava.s8     r8, q2, q3
7010bc6c: vpst          
7010bc70: vldrbt.u8     q2, [r2, #0]
7010bc74: vmlava.s8     sl, q0, q3
7010bc78: vaddva.s8     r0, q3
7010bc7c: vmlava.s8     r6, q1, q3
