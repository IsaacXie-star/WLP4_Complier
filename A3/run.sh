#! /bin/bash

clear
g++ -g -std=c++17 -Wall asm.cc scanner.cc -o asm
cs241.binasm < input.asm > expected.mips
./asm < input.asm > output.mips
diff output.mips expected.mips
echo "================================="
echo "----------Expected---------------"
cs241.binview expected.mips
echo " "
echo "-------------Get-----------------"
cs241.binview output.mips
echo " "