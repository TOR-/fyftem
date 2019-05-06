%include "dic.inc"

global _start

section .bss
resq 1023
rstack_start: resq 1
input_buf: resb input_buf_size

section .text

colon "main", main
	dq xt_number
	dq xt_number
	dq xt_div
	dq xt_dotS
	dq xt_bye

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
