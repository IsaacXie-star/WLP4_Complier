#! /bin/bash

cs241.binasm <<< '.word 0x10000' > address.bin
cs241.linkasm < input.asm > input.merl
cat address.bin input.merl > input.in
mips.stdin load.asm < input.in
