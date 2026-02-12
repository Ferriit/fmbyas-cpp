ldi r0, 72      ; 'H' in r0
str 43520, r0   ; y = 0, x = 1

ldi r0, 73      ; 'I' in r0
str 43522, r0   ; y = 0, x = 1

ldi r1, 8192    ; counter
ldi r2, 0       ; zero value

loop:
    dec r1
    cmp r1, r2  ; compare r1 with 0
    jnz loop

hlt
