.import print
.import init
.import new
.import delete
lis $5
.word Fwain
jr $5

Fwain:
;;;prolog
lis $4
.word 4
lis $11
.word 1
sub $29, $30, $4 ; init fp
;;;prolog done

sw $1, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4
lis $5
.word init
sw $31, -4($30)
sub $30, $30, $4
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $2, -4($30)
lis $3
.word 1
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
beq $3, $11, label0
add $1, $3, $0
lis $3
.word delete
sw $31, -4($30)
sub $30, $30, $4
jalr $3
add $30, $30, $4
lw $31, -4($30)
label0:
lw $3, 0($29)
sw $3, -8($29)
lw $3, -8($29)
lw $3, 0($3)
sw $1, -4($30)
sub $30, $30, $4
add $1, $0, $3
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word print
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $1, -4($30)
lis $3
.word -4
add $3, $3, $29
sw $3, -8($29)
lw $3, -8($29)
lw $3, 0($3)
sw $1, -4($30)
sub $30, $30, $4
add $1, $0, $3
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word print
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $1, -4($30)
lis $3
.word 0x1
sw $3, -8($29)
lw $3, -8($29)
beq $3, $11, label1
add $1, $3, $0
lis $3
.word delete
sw $31, -4($30)
sub $30, $30, $4
jalr $3
add $30, $30, $4
lw $31, -4($30)
label1:
lw $3, -8($29)
beq $3, $11, label2
add $1, $3, $0
lis $3
.word delete
sw $31, -4($30)
sub $30, $30, $4
jalr $3
add $30, $30, $4
lw $31, -4($30)
label2:
lis $3
.word 0

;;;epilog
add $30, $29, $4
jr $31
;;;epilog done

