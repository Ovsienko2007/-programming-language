    IN
    POP RAX

    CALL :1

    OUT
    HALT

:1
    PUSH RAX
    PUSH 0
    JB   :3  

    PUSH 0
    PUSH RAX
    JE   :20000

    PUSH RAX

    PUSH RAX
    PUSH 1
    SUB
    POP  RAX

    CALL :1
    MUL
    RET

:20000
    PUSH 1
    RET

:3
    PUSH 0
    RET