ldi r0, 0x48      ; 'H' in r0
str 0xAA00, r0    ; y = 0, x = 0

ldi r0, 0x49      ; 'I' in r0
str 0xAA02, r0    ; y = 0, x = 1

ldi r2, 80        ; comparison value
ldi r1, 0x0       ; counter

mov r4, sp
ldi sp, 47518

loop:
    pshi 0x0120    ; Push the immediate 0x120 (high 1 + ' ')
    inc r1
    cmp r1, r2
    jnz loop

; print message
ldi r0, 0x0148
str 47360, r0

ldi r0, 0x0149
str 47362, r0

ldi r0, 0x012E
str 47364, r0

ldi r0, 0x0153
str 47366, r0


ldi r2, 0x0       ; zero comparison value
ldi r1, 0x8000    ; counter

hltloop:
    dec r1
    cmp r1, r2
    jnz hltloop

hlt
