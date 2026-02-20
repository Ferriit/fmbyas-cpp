#!/usr/bin/env bash

# echo "Installing the FMBYAS assembler (fmasm) and emulator (fmemu)"
# 
# echo "Compiling fmemu and fmasm"
# mkdir dist/
# g++ src/emulator.cpp -o dist/fmemu O3 -march=native -flto -std=c++20 -fno-exceptions -fno-rtti -lncurses -DCURSESSUPPORT -DDISASSEMBLE
# g++ src/assembler.cpp -o dist/fmasm -O3 -march=native -flto -std=c++20 -fno-exceptions -fno-rtti
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
git clone https://github.com/ferriit/menumake-py --depth=1

cd ..
echo "Opening menu..."

python3 dist/menumake-py/src/main.py run build

echo "Cleaning up..."
rm -rf dist/

