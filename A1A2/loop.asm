;; Write a MIPS program which takes a non-negative integer
;; in $1 and stores n! (n factorial) in $3

lis $3
.word 1
lis $11
.word 1

loop: beq $1, $0, end
mult $3, $1
mflo $3
sub $1, $1, $11
beq $0, $0, loop 
end: jr $31
