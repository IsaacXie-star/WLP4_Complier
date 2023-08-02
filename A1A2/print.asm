;; simulate a stack to store
print:
;; store $1
sw $1, -4($30)
sw $2, -8($30)
sw $3, -12($30)
sw $4, -16($30)
sw $5, -20($30)
sw $6, -24($30)
sw $7, -28($30)
sw $8, -32($30)
lis $3
.word 32
lis $5
.word 48
lis $6
.word 4
sub $30, $30, $3
add $3, $30, $0
lis $2
.word 10
slt $7, $1, $0 ; check negativity
beq $7, $0, loop1
sub $1, $0, $1
loop1:
beq $1, $0, endloop1
div $1, $2
mflo $1
mfhi $4
add $4, $4, $5 ; convert to ASCII Char digit
sub $3, $3, $6
sw $4, 0($3)
beq $0, $0, loop1
endloop1:
lis $8
.word 0xffff0000
beq $7, $0, loop2
lis $1
.word 45
sw $1, 0x000c($8)
loop2:
beq $3, $30, endloop2
lw $1, 0($3)
sw $1, 0x000c($8)
add $3, $3, $6
beq $0, $0, loop2
endloop2:
lis $1
.word 10
sw $1, 0x000c($8)
lis $3
.word 32
add $30, $30, $3
lw $8, -32($30)
lw $7, -28($30)
lw $6, -24($30)
lw $5, -20($30)
lw $4, -16($30)
lw $3, -12($30)
lw $2, -8($30)
lw $1, -4($30)
jr $31
