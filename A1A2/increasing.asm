;; compare procedure returns 1 in $3 if the first param < second param
;; $5 store 4
;; $6 contains the end position of the array
;; $1 cur val, $2 nxt val, $8 cur, $9 nxt

main:
lis $5
.word 4
mult $2, $5
mflo $6
lis $7
.word compare
sub $6, $6, $5
add $6, $1, $6 ; $6 now stores the end position of the array
add $8, $0, $1
add $9, $8, $5
loop:
beq $8, $6, end
sw $31, -4($30)
lis $31
.word 4
sub $30, $30, $31
lw $1, 0($8)
lw $2, 0($9)
jalr $7
lis $31
.word 4
add $30, $30, $31
lw $31, -4($30)
beq $3, $0, end
add $8, $9, $0
add $9, $9, $5
beq $0, $0, loop
end: jr $31

compare:
slt $3, $1, $2
jr $31
