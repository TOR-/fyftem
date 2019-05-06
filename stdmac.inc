;syscall       rax ; rdi---> rsi-----> rdx------> rcx------> r8-> r9---->|return(rax)
;===================================================================================
SYS_READ  EQU 0    ; fd    . buf*    . count    .          .    .        |nread
SYS_WRITE EQU 1    ; fd    . buf*    . count    .          .    .        |nwritten
SYS_OPEN  EQU 2    ; name* . r/w flg . mode     .          .    .        |new fd
SYS_MMAP  EQU 9    ; addr  . len     . prot flg . util flg . fd . offset |area* 
SYS_EXIT  EQU 60   ; status.                                             | -

;no preferred start address for mmap
NO_HINT     EQU  0
;protection flags for SYS_MMAP
%define PROT_READ  0X1
%define PROT_WRITE 0x2
%define PROT_EXEC  0x4
%define PROT_NONE  0x0
;util flags for SYS_MMAP
MAP_SHARED  EQU  0x1
MAP_PRIVATE EQU  0x2
MAP_ANON    EQU  0x20
MAP_FIXEED  EQU  0x10
;modes for SYS_OPEN
O_RDONLY    EQU 0
O_WRONLY    EQU 1
O_RDWR      EQU 2
;std fd
STD_IN      EQU 0
STD_OUT     EQU 1
STD_ERR     EQU 2
;characters
NULL        EQU 0
LF          EQU 0xA
TAB         EQU 0x9
SPC         EQU 0x20

ASCII_OFFS  EQU '0'  ; 0x30
