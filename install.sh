#!/bin/env bash

echo "Installing the FMBYAS assembler (fmasm) and emulator (fmemu)"

echo "Compiling fmemu and fmasm"
mkdir dist/
g++ src/emulator.cpp -o dist/fmemu -lncurses -O3 -march=native -DCURSESSUPPORT -DDISASSEMBLE
g++ src/assembler.cpp -o dist/fmasm -O3 -march=native

sudo chmod +x dist/fmemu
sudo chmod +x dist/fmasm

sudo mv dist/fmemu /usr/local/bin/fmemu
sudo mv dist/fmasm /usr/local/bin/fmasm

echo "Installation completed"
