a       lw      0       6       A
        lw      0       7       B
E       lw      0       7       a
G       sw      0       3       C
        sw      0       0       D
        sw      0       7       a
        sw      0       0       Stack
        lw      0       6       Stack
        beq     4       2       junkk
        beq     4       2       junk
Start   beq     4       2       Junk
start   beq     2       3       Start
        beq     0       0       Start
        add     1       0       1
        beq     2       3       start 
        beq     0       0       start
        halt
junk    .fill   312
junkk   .fill   39012
Junk    .fill   312
F       .fill   junk
H       .fill   junkk