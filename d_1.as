b       lw      0       6       E
        lw      0       7       F
A       lw      0       7       b
C       sw      0       3       G
        sw      0       0       H
        sw      0       7       b
        sw      0       0       Stack
        lw      0       6       Stack
        beq     4       2       junk2
        beq     4       2       junk1
Start1  beq     4       2       Junk1
start1  beq     2       3       Start1
        beq     0       0       Start1
        add     1       0       1
        beq     2       3       start1 
        beq     0       0       start1
        halt
junk1   .fill   312
junk2   .fill   39012
Junk1   .fill   312
B       .fill   junk1
D       .fill   junk2