stirling:
sw $1, -4($30)
sw $2, -8($30)
sw $4, -12($30)
sw $5, -16($30)
sw $6, -20($30)
sw $7, -24($30)
sw $31, -28($30)
lis $4
.word 28
sub $30, $30, $4
lis $7
.word 1

;; base case
;; $1 = 0, $2 = 0
if1:
bne $1, $0, if2
bne $2, $0, else1
lis $3 ; $1 = 0, $2 = 0
.word 1
beq $0, $0, end
else1: ; $1 = 0, $2 > 0
lis $3
.word 0
beq $0, $0, end
if2:
bne $2, $0, else2
lis $3 ; $1 > 0, $2 = 0
.word 0
beq $0, $0, end
else2: ; $1 > 0, $2 > 0
lis $4
.word stirling
sub $1, $1, $7
jalr $4
add $5, $3, $0
sub $2, $2, $7
jalr $4
add $6, $3, $0
mult $1, $5
mflo $3
add $3, $3, $6 ; change $3 to correct value
beq $0, $0, end

end:
lis $4
.word 28
add $30, $30, $4
lw $31, -28($30)
lw $7, -24($30)
lw $6, -20($30)
lw $5, -16($30)
lw $4, -12($30)
lw $2, -8($30)
lw $1, -4($30)
jr $31