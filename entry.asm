%include "dic.inc"

global _start

section .bss
resq 1023
rstack_start: resq 1
input_buf: resb 1024

section .text

; one cell program
main_stub: dq xt_main

; ============
; INTERPRETER
; ============
next:
	mov w, [pc]
	add pc, 8
	jmp [w]

_start: jmp i_init
