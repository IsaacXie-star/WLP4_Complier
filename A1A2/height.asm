;; $1 holds current address we are at
;; $2 holds the size of the array
;; $3 stores the answer, $9 holds constant 1, $10 holds constant -1
;; $11 holds the starting address of the array
;; $15 holds the constant 4

main:
lis $9
.word 1
lis $10
.word -1
lis $15
.word 4
add $11, $0, $1
lis $8
.word getHeight
sw $31, -4($30)
lis $31
.word 4
sub $30, $30, $31
jalr $8
lis $31
.word 4
add $30, $30, $31
lw $31, -4($30)
jr $31

getHeight:
sw $1, -4($30)
sw $2, -8($30)
sw $4, -12($30)
sw $5, -16($30)
sw $6, -20($30)
sw $7, -24($30)
sw $9, -28($30)
sw $10, -32($30)
sw $11, -36($30)
sw $12, -40($30)
sw $13, -44($30)
sw $14, -48($30)
sw $31, -52($30)
lis $4
.word 52
sub $30, $30, $4

;; base case, $6 = -1, $7 = -1
lw $6, 4($1)
lw $7, 8($1)
if1:
bne $6, $10, if2
bne $7, $10, else1
lis $3 ; $6 = -1, $7 = -1
.word 1
beq $0, $0, getHeightEnd
else1: ; $6 = -1, $7 != -1
mult $7, $15
mflo $7
add $1, $11, $7
jalr $8
add $3, $3, $9 
beq $0, $0, getHeightEnd
if2:
bne $7, $10, else2
lis $3 ; $6 != -1, $7 = -1
.word 0
mult $6, $15
mflo $6
add $1, $11, $6
jalr $8
add $3, $3, $9
beq $0, $0, getHeightEnd
else2: ; $6 != -1, $7 != -1
mult $6, $15
mflo $6
add $1, $11, $6
jalr $8
add $12, $3, $9
mult $7, $15
mflo $7
add $1, $11, $7
jalr $8
add $13, $3, $9
slt $14, $12, $13
bne $14, $0, useSecond
add $3, $0, $12
beq $0, $0, swapEnd
useSecond:
add $3, $0, $13
beq $0, $0, swapEnd
swapEnd:
beq $0, $0, getHeightEnd
getHeightEnd:
lis $4
.word 52
add $30, $30, $4
lw $31, -52($30)
lw $14, -48($30)
lw $13, -44($30)
lw $12, -40($30)
lw $11, -36($30)
lw $10, -32($30)
lw $9, -28($30)
lw $7, -24($30)
lw $6, -20($30)
lw $5, -16($30)
lw $4, -12($30)
lw $2, -8($30)
lw $1, -4($30)
jr $31