;; An integer m divides an integer n if there exists an integer k such that n = km
;; interprets the values in $1 and $2 as unsigned integers
;; If $1 divides $2, place 1 in $3, otherwise place 0 in $3

bne $1, $0, 7
beq $2, $0, 3
lis $3
.word 0
beq $0, $0, 11
lis $3
.word 1
beq $0, $0, 8
divu $2, $1
mfhi $3
bne $3, $0, 3
lis $3
.word 1
beq $0, $0, 2
lis $3
.word 0

jr $31