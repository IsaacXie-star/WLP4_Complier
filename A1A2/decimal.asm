;; $4 stores 0xffff0000, $3 stores pos/neg (0, 1), $2 store value, $1 loading
;; $5 stores 10, $6 stores ASCII offset, $7 stores 0x0A

lis $2
.word 0
lis $3
.word 45
lis $5
.word 10
lis $6
.word 48
lis $7
.word 0x0A
lis $4
.word 0xffff0000
lw $1, 0x0004($4)
;; first check '-' sign
bne $1, $3, 3
lis $3
.word 1
beq $0, $0, 3
lis $3
.word 0
sub $2, $1, $6
lw $1, 0x0004($4)
beq $1, $7, 5
mult $2, $5
mflo $2
sub $8, $1, $6
add $2, $8, $2
beq $0, $0, -7
beq $3, $0, 4
lis $3
.word -1
mult $3, $2
mflo $2
sw $2, 0x000c($4)
jr $31