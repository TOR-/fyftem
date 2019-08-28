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
msg_stack_empty:
  db "stack empty", 0
prompt:
  db "€ ", 0
state: dq INTERPRET

section .text

main_stub: 
  dq xt_quit

next:
  mov w, [pc]
  add pc, CELL_SIZE
  jmp [w]

_start: jmp i_init


