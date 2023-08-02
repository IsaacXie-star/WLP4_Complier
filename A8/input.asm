add $13, $31, $0
lis $31
.word 31
lis $11
.word 1
sltu $3, $1, $2
sub $3, $11, $3
add $31, $31, $3
jr $13
.word 0xCa7Cafe