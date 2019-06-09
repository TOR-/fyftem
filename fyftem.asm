%include "dic.inc"

global _start

section .bss
resq 1023
rstack_start: resq 1
input_buf: resb input_buf_size

pad: resq 1024

section .data
msg_undefined:
  db "undefined word", 0
next_xt: dq 0
state: dq INTERPRET

section .text

main_stub: 
;.loop
  dq xt_quit
;  dq xt_lit, LF, xt_emit
;  dq branch0, .loop
;  dq xt_bye

; ============
; INTERPRETER
; ============
colon "quit", quit
.loop:
  dq xt_bl, xt_word ; ( caddr len)
  dq xt_drop, xt_interpret
  dq xt_branch, .loop
  dq xt_bye

; ( caddr len --)
colon "interpret", interpret
  dq xt_dup, xt_find ; ( caddr addr/0 )
  dq xt_dup, xt_branch0, .number
  dq xt_cfa, xt_execute
  dq xt_exit
.number:  ; ( caddr 0)
  dq xt_drop, xt_dup, xt_number  ; ( caddr num len/0)
  dq xt_branch0, .undefined
  dq xt_swap, xt_drop
  dq xt_exit
.undefined: ; ( caddr 0)
  dq xt_drop
  dq xt_lit, '[', xt_emit, xt_lit, msg_undefined, xt_count, xt_type, xt_lit, SPC, xt_emit
  dq xt_lit, '"', xt_emit, xt_count, xt_type, xt_lit, '"', xt_emit, xt_lit, ']', xt_emit
  dq xt_exit

next:
  mov w, [pc]
  add pc, CELL_SIZE
  jmp [w]

_start: jmp i_init


