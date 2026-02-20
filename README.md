# fmbyas-cpp

This is an implementation of the fmbyas-2 CPU architecture spec viewable in [FMBYAS-2 SPEC](fmbyas-2_spec.md) in C++.

This repo contains both the FMASM assembler and FMEMU emulator.

---

## Installation:
Currently there's only a Linux installer in the form of `install.sh`.


### Dependencies:
- Python3.12+
- g++ (GCC)
- (ncurses)

### (Notes:)
(This was originally only meant as a tool I could develop and use myself. Therefore there's no standard windows or MacOS installation script currently.)

## Usage:
- To assemble a program with FMASM, run `fmasm [file]`. This supports both absolute and relative jumps where absolute jumps are default and relative jumps are enabled with the `-r` option or toggled with the `TJF` instruction. This generates an out.bin file.

- To run the emulator, run `fmemu` in the same directory as the out.bin file. Make sure you compiled it with the correct flags in the config menu.
- For details on how screen and keyboard support works, RTFM [FMBYAS-2 SPEC](fmbyas-2_spec.md)

---
