nop
nop
# put b-vals
chr r0 0xb0
chr r1 0xb1
chr r2 0xb2
chr r3 0xb3
chr r4 0xb4
chr r5 0xb5
chr r6 0xb6
chr r7 0xb7
# put them in the same spots at the same time
str r0 0x0
str r1 0x1
str r2 0x2
str r3 0x3
str r4 0x10000000
str r5 0x10000001
str r6 0x10000002
str r7 0x10000003
# load them back
ldr r0 0x0
ldr r1 0x1
ldr r2 0x2
ldr r3 0x3
ldr r4 0x10000000
ldr r5 0x10000001
ldr r6 0x10000002
ldr r7 0x10000003
# see what happened
dmp
# set gpios to 1
chr r0 0x1
str r0 0x10000000
str r0 0x10000001
str r0 0x10000002
str r0 0x10000003
str r0 0x10000004
str r0 0x10000005
str r0 0x10000006
str r0 0x10000007
# load them back
ldr r0 0x10000000
ldr r1 0x10000001
ldr r2 0x10000002
ldr r3 0x10000003
ldr r4 0x10000004
ldr r5 0x10000005
ldr r6 0x10000006
ldr r7 0x10000007
# see what happened
dmp