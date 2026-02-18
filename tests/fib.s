_start:
    ldi r0 1        ; a
    ldi r1 1        ; b
    ldi r2 24       ; amount of times to repeat
    ldi r3 0        ; Zero counter

    fib:
        mov r0 io0
        add io0 r1      ; a_new = a_old + b_old
        
        mov r1 r0       ; b_new = a_old

        mov r0 io0

        dec r2          ; decrement r2

        cmp r2 r3       ; compare r2 with 0
        jnz fib


    hlt
