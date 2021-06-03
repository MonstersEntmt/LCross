mov al, 10                       ; B00A

SomeByte: db 7Fh                 ; 7F
SomeBytes: db 3Fh, 3Fh           ; 3F3F
SomeWord: dw -653                ; 73FD
SomeWords: dw -3, 8888           ; FDFFB822
SomeDWord: dd 55                 ; 37000000
SomeDWords: dd 667674, -18723871 ; 1A300A00E14BE2FE
SomeQWord: dq 15                 ; 0F00000000000000
SomeQWords: dq 15, -653          ; 0F0000000000000073FDFFFFFFFFFFFF

resb 16                          ; 00000000000000000000000000000000
resw 8                           ; 00000000000000000000000000000000
resd 4                           ; 00000000000000000000000000000000
resq 2                           ; 00000000000000000000000000000000

mov ax, -6512                    ; B890E6             | 66B890E6
mov byte[SomeByte], al           ; A20200             | 88042502000000
mov byte[SomeBytes], al          ; A20300             | 88042503000000
mov word[SomeWord], ax           ; A30500             | 6689042505000000
mov word[SomeWords], ax          ; A30700             | 6689042507000000
mov dword[SomeDWord], eax        ; 66A30B000000       | 8904250B000000
mov dword[SomeDWords], eax       ; 66A30F000000       | 8904250F000000
;mov qword[SomeQWord], rax       ; A31700000000000000 | 4889042517000000
;mov qword[SomeQWords], rax      ; A31F00000000000000 | 488904251F000000
mov byte[127], al                ; A27F	              | 8804257F000000
mov word[16], ax                 ; A31000             | 6689042510000000
mov dword[3434434], eax          ; 66A3C2673400       | 890425C2673400
;mov qword[789128371], rax       ; A3B324092F         | 48890425B324092F