800711e: sxtb16       ip, r0, ror #8
8007122: ldr          r5, [r6, #0]
8007124: sxtb16       r0, r0
8007128: smlad        r5, r5, r0, r3
800712c: ldr          r3, [r6, #4]
800712e: adds         r6, #8
8007130: smlad        r3, r3, ip, r5
8007134: cmp          r9, r4
8007136: bne.n        800711a <st_sssa8_fully_connected+0xfa>
8007138: ldr          r5, [sp, #32]
