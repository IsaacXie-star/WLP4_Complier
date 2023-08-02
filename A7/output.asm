;;;prolog
.import print
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
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
add $30, $30, $4
lw $5, -4($30)
add $3, $3, $5
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
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
add $30, $30, $4
lw $5, -4($30)
add $3, $3, $5

;;;epilog
add $30, $29, $4
jr $31
;;;epilog done
