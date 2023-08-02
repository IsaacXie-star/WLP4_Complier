;; Copy the value in register 1 to register 2
;; Store the twos compement encoding of -241 in register 4
;; Add the value in register 2 to the value in register 4, storing the result in register 1
;; Terminate the program by returning to the loader

lis $4
.word -241

add $2, $1, $0
add $1, $2, $4

jr $31