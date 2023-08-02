;; use $1 to store input, $3 to count, $2 to store -1, $4 to store 1, $5 to store 0xffff0000

lis $2
.word -1
lis $4
.word 1
lis $5
.word 0xffff0000
add $3, $0, $0
lw $1, 0x0004($5)
beq $1, $2, 3
add $3, $3, $4
sw $1, 0x000c($5)
beq $0, $0, -5
jr $31
