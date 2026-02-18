_start:
    ldi r1, 0xAA00      ; write position
    ldi r0, 0           ; character
    ldi r2, 0xFFFF      ; threshold for drawing
    ldi r3, 27          ; escape character

    ldi r4, 2           ; 2

    ldi r5, 0x00FF      ; character mask
    ldi r6, 0xFF00      ; metadata mask

    loop:
        ldi io7, 0x0200 ; ask for the current character
        mov r0, io7
        and r0, r5      ; current character in r0

        mov r7, r6
        and r7, io7     ; current metadata in r7

        ldi r8, 0x0100  ; new key, jump

        cmp r7, r8
        jnz resume      ; don't draw key if it isn't new
        
        rstr r1, r0     ; draw character

        add r1, r4      ; increment r1

        resume:

        cmp r0, r3
        jnz loop        ; loop if escape wasn't pressed

    hlt
