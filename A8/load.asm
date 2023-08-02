main:
lis $4
.word 4
lis $6
.word readWord
lis $7
.word printHex
sw $31, -4($30)
sub $30, $30, $4
jalr $6
add $30, $30, $4
lw $31, -4($30)
lis $5
.word 0x10004
add $30, $3, $5
sw $31, -4($30)
sub $30, $30, $4
add $10, $3, $0
add $11, $10, $0
add $14, $10, $0
jalr $6
jalr $6
add $16, $3, $0
jalr $6
add $8, $3, $0
lis $9
.word 0x0c
loop1:
jalr $6
beq $9, $8, end1
add $1, $3, $0
sw $3, 0($11)
add $11, $11, $4
jalr $7
add $9, $9, $4
beq $0, $0, loop1
end1:
loop3:
beq $9, $16, end3
add $9, $9, $4
jalr $6
add $15, $14, $3
lw $18, 0($15)
add $18, $18, $14
sw $18, 0($15)
add $9, $9, $4
jalr $6
beq $0, $0, loop3
end3:
sw $31, -4($30)
sub $30, $30, $4
jalr $10
lis $4
.word 4
add $30, $30, $4
lw $31, -4($30)

loop2:
beq $10, $11, end2
lw $1, 0($10)
add $10, $10, $4
jalr $7
beq $0, $0, loop2
end2:
add $30, $30, $4
lw $31, -4($30)
jr $31

readWord:
sw $1,  -4($30)
sw $2,  -8($30)
sw $4, -12($30)
sw $5, -16($30)
sw $6, -20($30)
sw $7, -24($30)
sw $8, -28($30)
lis $8
.word 28
sub $30, $30, $8
lis $4
.word 0x01000000
lis $3
.word 0x00010000
lis $2
.word 0x00000100
lis $1
.word 0xffff0004
lw $8, 0($1)
lw $7, 0($1)
lw $6, 0($1)
lw $5, 0($1)
multu $8, $4
mflo $8
multu $7, $3
mflo $7
multu $6, $2
mflo $6
add $4, $8, $7
add $3, $6, $5
add $3, $4, $3
lis $8
.word 28
add $30, $30, $8
lw $1,  -4($30)
lw $2,  -8($30)
lw $4, -12($30)
lw $5, -16($30)
lw $6, -20($30)
lw $7, -24($30)
lw $8, -28($30)
jr $31

printHex:
sw $1,  -4($30)
sw $2,  -8($30)
sw $3, -12($30)
sw $4, -16($30)
sw $5, -20($30)
sw $6, -24($30)
sw $7, -28($30)
sw $8, -32($30)
lis $8
.word 32
sub $30, $30, $8
lis $2
.word 0x10000000
lis $3
.word 0x10
lis $4
.word 0x30
lis $6
.word 10
lis $7
.word 7  
lis $8
.word 0xffff000c
PHloop:
divu $1, $2
mflo $1
slt $5, $1, $6 
bne $5, $0, PHskip
        add $1, $1, $7
PHskip: add $1, $1, $4 
sw $1, 0($8)
mfhi $1
divu $2, $3
mflo $2
bne $2, $0, PHloop
sw $6, 0($8)
lis $8
.word 32
add $30, $30, $8
lw $1,  -4($30)
lw $2,  -8($30)
lw $3, -12($30)
lw $4, -16($30)
lw $5, -20($30)
lw $6, -24($30)
lw $7, -28($30)
lw $8, -32($30)
jr $31