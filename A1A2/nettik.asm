;; use $1 to store input, $3 to count, $2 to store -1, $4 to store 1, $5 to store 0xffff0000, $6 store offset
;; use memory to simulate a queue to store the chars

lis $2
.word -1
lis $4
.word 1
lis $5
.word 0xffff0000
lis $7
.word 4
add $6, $0, $30
add $3, $0, $0
loop1:
lw $1, 0x0004($5)
beq $1, $2, 4
add $3, $3, $4
sub $6, $6, $7
sw $1, 0($6)
beq $0, $0, loop1

loop2:
beq $6, $30, end
lw $1, 0($6)
sw $1, 0x000c($5)
add $6, $6, $7
beq $0, $0, loop2

end: jr $31