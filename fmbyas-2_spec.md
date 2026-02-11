# FMBYAS
### (Pronounced "Femboy Ass")
## Formattable Multi-architecture Based Yield Assembly

## OPCODES:
### Register and Memory Control:
```
LDI = Load immediate into register                                  (ldi [register] [value])
MOV = Move value from register A to B                               (mov [register B] [register A])
LD = Load value from RAM into register                              (ld [register] [addr])
STR = Store value from register into RAM                            (str [addr] [register])
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
CALLR = Calls the function that a register is pointing to           (callr [register])
RET = Returns execution from the last caller                        (ret)
```

### Misc:
```
NOP = No-op. Doesn't do anything                                    (nop)
HLT = Halts execution                                               (hlt)
WAIT = Halts for the amount of cycles described by a register       (wait [register])
WAITI = Halts for a certain amount of cycles                        (waiti [value])
CONT = Continue execution in RAM                                    (cont) (Compiles, but doesn't run in FMBYAS-2)
```

## REGISTERS:
```
r0 - rn: General purpose registers
pc: Program counter. What the index of the currently executed instruction is
stackptr: The length of the stack
io0 - io7: I/O registers. io0 is for reading/writing to harddrive and io1 is for harddrive flag

fz: Zero flag
fn: Negative flag
fg: Greater flag
```

## Quickscan list:
```
LDI = Load immediate into register                                  (ldi [register] [value])
MOV = Move value from register A to B                               (mov [register] [register])
LD = Load value from RAM into register                              (ld [register] [addr])
STR = Store value from register into RAM                            (str [addr] [register])
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
```
