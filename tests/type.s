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
        jnz continue    ; don't draw key if it isn't new 
        
        ldi r3, 8       ; backspace character
        cmp r3, r0      ; compare backspace to r0
        jz backspace    ; skip drawing new character, clear and move back

        rstr r1, r0     ; draw character

        add r1, r4      ; increment r1
        jnz continue    ; skip backspace logic

        backspace:
            sub r1, r4       ; move back first
            ldi r0, 0
            rstr r1, r0      ; clear current position
            jmp continue

        continue:
        ldi r3, 0xAA00  ; minimum screen position
        cmp r1, r3
        jlt reset_r1  ; restore r1 if r1 < 0xAA00
        jmp skip_reset
        
        reset_r1:
            ldi r1, 0xAA00
        skip_reset:
    
        ldi r3, 27      ; put escape character back in r3

        cmp r0, r3
        jnz loop        ; loop if escape wasn't pressed

    hlt
