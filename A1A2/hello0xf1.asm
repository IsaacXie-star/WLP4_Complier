;; Copy the value in register 1 to register 2
;; Store the twos compement encoding of -241 in register 4
;; Add the value in register 2 to the value in register 4, storing the result in register 1
;; Terminate the program by returning to the loader
;; Only use .word directive

;; add $2, $1, $0, 0000 0000 0010 0000 0001 0000 0010 0000
.word 2101280

;; lis $4 0000 0000 0000 0000 0010 0000 0001 0100
.word 8212
.word -241
;; add $1, $2, $4 0000 0000 0100 0100 0000 1000 0010 0000
.word 4458528

;; jr $31 0000 0011 1110 0000 0000 0000 0000 1000
.word 65011720