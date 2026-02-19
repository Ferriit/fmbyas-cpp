#!/bin/env bash

set -e

echo "Installing the FMBYAS assembler (fmasm) and emulator (fmemu)"

echo "Compiling fmemu and fmasm"
mkdir -p dist/
g++ src/emulator.cpp -o dist/fmemu $MENUMAKE_OPTIONS_0
g++ src/assembler.cpp -o dist/fmasm $MENUMAKE_OPTIONS_1

sudo chmod +x dist/fmemu
sudo chmod +x dist/fmasm

sudo mv dist/fmemu /usr/local/bin/fmemu
sudo mv dist/fmasm /usr/local/bin/fmasm

echo "Installation completed"
