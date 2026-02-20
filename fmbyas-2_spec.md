# FMBYAS
## Formattable Multi-architecture Based Yield Assembly

## OPCODES:
### Register and Memory Control:
```
LDI = Load immediate into register                                  (ldi [register] [value])
MOV = Move value from register A to B                               (mov [register B] [register A])
LD = Load value from RAM into register                              (ld [register] [addr])
STR = Store value from register into RAM                            (str [addr] [register])
RLD = Load value from RAM address stored in register into register  (rld [register] [register])
RSTR = Store value from register to RAM address stored in register  (rstr [register] [register])
XCHG = Swaps the contents of two registers                          (xchg [register] [register])
PSH = Pushes a register value onto the stack                        (psh [register])
PSHI = Pushes an immediate value onto the stack                     (pshi [value])
POP = Pops the top-most value on the stack into the register        (pop [register])
PEK = Peeks the top-most value on the stack into a regsiter         (pek [register])
SRMV = Removes the top-most value on the stack (stack remove)       (srmv)
SWP = Swaps the values between register and RAM                     (swp [addr] [register])
LEA = Loads the address of a value into a register                  (lea [register] [addr])
```

### Arithmetic:
#### Everything saves in the first mentioned register
```
ADD = Add contents of registers                                     (add [register] [register])
SUB = Subtract contents of registers                                (sub [register] [register])
MUL = Multiply contents of registers                                (mul [register] [register])
DIV = Divide contents of registers                                  (div [register] [register])
INC = Increment contents of registers                               (inc [register])
DEC = Decrement contents of registers                               (dec [register])
AND = Performs a bitwise AND operation on both registers            (and [register] [register])
OR = Performs a bitwise OR operation on both registers              (or [register] [register])
NOT = Performs a bitwise NOT on one register                        (not [register])
XOR = Performs a bitwise XOR between registers                      (xor [register] [register])
SHL = Logical bitshift left                                         (shl [register] [shift])
SHR = Logical bitshift right                                        (shr [register] [shift])
RSL = Rotate bits in register left                                  (rsl [register] [shift])
RSR = Rotate bits in register right                                 (rsr [register] [shift])
```

### Flow Control:
```
CMP = Compares two registers. Stores the appropriate flags          (cmp [register] [register])
JMP = Jumps to a label                                              (jmp [label name])
JZ = Jumps if the zero flag is set (equal)                          (je [label name])
JNZ = Jumps if the zero flag isn't set (not equal)                  (jne [label name])
JGT = Jumps if the greater flag is set                              (jgt [label name])
JLT = Jumps if the negative flag is set (less than)                 (jtl [label name])
JGE = Jumps if the negative flag isn't set (greater than or equal)  (jge [label name])
JLE = Jumps if the greater flag isn't set (lesser than or equal)    (jle [label name])
CALL = Calls a function that saves to stack when returned           (call [label name])
CALLR = Calls the function that a register is pointing to           (callr [register]) (deprecated)
RET = Returns execution from the last caller                        (ret)
```

### Misc:
```
NOP = No-op. Doesn't do anything                                    (nop)
HLT = Halts execution                                               (hlt)
WAIT = Halts for the amount of cycles described by a register       (wait [register])
WAITI = Halts for a certain amount of cycles                        (waiti [value])
CONT = Continue execution in RAM                                    (cont) (Compiles, but doesn't run in FMBYAS-2)
TJF = Toggles the fj flag between 0 and 1                           (tjf)
```
- All instructions follow the format `OPCODE <DEST> <SRC>`

## REGISTERS:
```
r0 - rn: General purpose registers
pc: Program counter. What the index of the currently executed instruction is
stackptr: The length of the stack
io0 - io7: I/O registers. io0 is for reading/writing to harddrive and io1 is for harddrive flag

FLAGS ARE NOT ACCESSABLE USING LDI, MOV...
fz: Zero flag
fn: Negative flag
fg: Greater flag

fj: Jump flag. 0 is Absolute Jumping, 1 is Relative Jumping
```

## Quickscan list:
```
LDI = Load immediate into register                                  (ldi [register] [value])
MOV = Move value from register A to B                               (mov [register] [register])
LD = Load value from RAM into register                              (ld [register] [addr])
STR = Store value from register into RAM                            (str [addr] [register])
RLD = Load value from RAM address stored in register into register  (rld [register] [register])
RSTR = Store value from register to RAM address stored in register  (rstr [register] [register])
XCHG = Swaps the contents of two registers                          (xchg [register] [register])
PSH = Pushes a register value onto the stack                        (psh [register])
PSHI = Pushes an immediate value onto the stack                     (pshi [value])
POP = Pops the top-most value on the stack into the register        (pop [register])
PEK = Peeks the top-most value on the stack into a regsiter         (pek [register])
SRMV = Removes the top-most value on the stack (stack remove)       (srmv)
SWP = Swaps the values between register and RAM                     (swp [addr] [register])
ADD = Add contents of registers                                     (add [register] [register])
SUB = Subtract contents of registers                                (sub [register] [register])
MUL = Multiply contents of registers                                (mul [register] [register])
DIV = Divide contents of registers                                  (div [register] [register])
INC = Increment contents of registers                               (inc [register])
DEC = Decrement contents of registers                               (dec [register])
AND = Performs a bitwise AND operation on both registers            (and [register] [register])
OR = Performs a bitwise OR operation on both registers              (or [register] [register])
NOT = Performs a bitwise NOT on one register                        (not [register])
XOR = Performs a bitwise XOR between registers                      (xor [register] [register])
SHL = Logical bitshift left                                         (shl [register] [shift])
SHR = Logical bitshift right                                        (shr [register] [shift])
RSL = Rotate bits in register left                                  (rsl [register] [shift])
RSR = Rotate bits in register right                                 (rsr [register] [shift])
CMP = Compares two registers. Stores the appropriate flags          (cmp [register] [register])
JMP = Jumps to a label                                              (jmp [label name])
JZ = Jumps if the zero flag is set (equal)                          (je [label name])
JNZ = Jumps if the zero flag isn't set (not equal)                  (jne [label name])
JGT = Jumps if the greater flag is set                              (jgt [label name])
JLT = Jumps if the negative flag is set (less than)                 (jtl [label name])
JGE = Jumps if the negative flag isn't set (greater than or equal)  (jge [label name])
JLE = Jumps if the greater flag isn't set (lesser than or equal)    (jle [label name])
CALL = Calls a subroutine that saves to stack when returned         (call [label name])
CALLR = Calls the function that a register is pointing to           (callr [register])
RET = Returns execution from the last caller                        (ret)
NOP = No-op. Doesn't do anything                                    (nop)
HLT = Halts execution until resume button is pressed                (hlt)
WAIT = Halts for the amount of cycles described by a register       (wait [register])
WAITI = Halts for a certain amount of cycles                        (waiti [value])
CONT = Continue execution in RAM                                    (cont)
TJF = Toggles the fj flag between 0 and 1                           (tjf)
```


## Emulator spec:
### Memory:
 - The emulator should have exactly 64K of space
 - The stack should start att 0xFFFF and then decrement
 - When writing to RAM, the high bits should get put in the address and the low bits should be put in the address + 1.
 - The program written by the user should be put at address 0x0000 from ROM.
 - The FJ flag should be set to 0 (absolute jumping) as default.

### Screen:
 - When printing to the screen, an ascii character should be put in the low bits of a register and write to memory location 0xAA00 + 2 * (x + y * 80) (The max location is therefore 0xB9A0).
 - The screen size should be 80x25.

### Keyboard:
 - Keyboard input is handled through io7.
 - The high byte is what the CPU writes to to get input from the keyboard. It fetches the most current keypress. This makes the emulator completely overwrite the io7 register. The outputted value has to be greater than 511 and the first bit of the high byte represents whether or not the key has updated since last request.
 - The low byte represents the key being pressed. Follows ASCII for most characters, but some don't.
 - All keycodes are returned as their ASCII codes and are modified depending on Shift (a (97) + [SHIFT] -> A (65), 1 (49) + [SHIFT] -> ! (32)) except the following list.
```
[BACKSPACE] - 8
[TAB] - 9
[RETURN] - 10
[ARROWUP] - 17
[ARROWDOWN] - 18
[ARROWLEFT] - 19
[ARROWRIGHT] - 20
[ESCAPE] - 27
[SPACE] - 32
[DEL] - 127
```

### Arguments:
- `--polling` - The polling rate of the keyboard (1000 means once every 1000 cycles). 1000 is the standard value.
- `--registers` - Sets the amount of general purpose registers (2 allows for r0, r1. 16 for r0 - r15, 32 for r0 - r31...). The standard value is 16.

## Assembler spec:
- Always has to take in the file as the first argument.
- `-r` is the second argument and if it's present, it makes the assembler start with relative jumping instead of absolute.
- The assembler should always insert a `jmp _start` instruction before everything else to make sure it starts at the right spot.
- Each assembler should omit an ID byte between `jmp _start` and `_start:` and can put any message after the ID byte.
- The assembler should support `db` for defining bytes.
