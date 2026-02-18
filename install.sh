#!/bin/env bash

# echo "Installing the FMBYAS assembler (fmasm) and emulator (fmemu)"
# 
# echo "Compiling fmemu and fmasm"
# mkdir dist/
# g++ src/emulator.cpp -o dist/fmemu -lncurses -O3 -march=native -DCURSESSUPPORT -DDISASSEMBLE
# g++ src/assembler.cpp -o dist/fmasm -O3 -march=native
# 
# sudo chmod +x dist/fmemu
# sudo chmod +x dist/fmasm
# 
# sudo mv dist/fmemu /usr/local/bin/fmemu
# sudo mv dist/fmasm /usr/local/bin/fmasm
# 
# echo "Installation completed"

echo "Temporarily downloading menumake..."
mkdir dist/
cd dist
rm -rf *
git clone https://github.com/ferriit/menumake-py

cd ..
echo "Opening menu..."

python3 dist/menumake-py/src/main.py && python3 dist/menumake-py/src/main.py build

echo "Cleaning up..."
rm -rf dist/

