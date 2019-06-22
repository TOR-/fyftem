%ifndef _FYFTEM_DICTIONARY_INC
%define _FYFTEM_DICTIONARY_INC

%include "stdmac.inc"
%include "macros.inc"
%include "iolib.inc"

section .data
data_stack_base: ; store data stack start address
  dq 0
dict_top:
  dq 0

section .text
; ============
; dictionary
; ============
first_word:
; initialises registers
native "init", init
  mov rstack, rstack_start ; reset return stack
  mov pc, main_stub        ; load entry point
  cmp qword [dict_top], 0  ; dict_top defined?
  jne .stack
  mov qword [dict_top], STATIC_DICT_END
.stack:
  cmp qword [data_stack_base], 0  ; initialised yet?
  jne .reset_stack
  mov [data_stack_base], rsp
  jmp next
.reset_stack:
  mov rsp, [data_stack_base]      ; reset data stack
  jmp next

; saves pc at start of colon word
native "docol", docol
  sub rstack, CELL_SIZE
  mov [rstack], pc
  add w, CELL_SIZE
  mov pc, w
  jmp next

; returns from colon word
native "exit", exit
  mov pc, [rstack]
  add rstack, CELL_SIZE
  jmp next

; ( char "<char>*ccc<char>" -- c-addr len)
; skip leading <char>s
; read ccc into buffer
; c-addr = buffer*
native "word", word
  pop rdx
  mov rdi, input_buf
  mov rsi, input_buf_size
  call read_word_delim
  push rdi
  push rax
  jmp next

; (c-addr1 -- c-addr1 len)
; convert from null-terminated string format to counted string
native "count", count
  pop rdi
  push rdi
  call string_length
  push rax
  jmp next

; (c-addr u --)
native "type", type
  mov rax, SYS_WRITE
  mov rdi, STD_OUT
  pop rdx
  pop rsi
  syscall
  jmp next
  
; ( str* -- ) 
; prints str
;native "prints", prints
;  pop rdi
;  call print_string
;  jmp next

; exits program
native "bye", bye
  mov rax, 60
  xor rdi, rdi
  syscall

; ( -- @input_buf )
native "inbuf", inbuf
  push qword input_buf
  jmp next

; ( name* -- codeaddr )
native "cfa", cfa
  mov rdx, rdi
  pop rdi
;  lea rdi, [rdi + CELL_SIZE]
  push rdx
  call string_length
  pop rdx
  add rax, rdi
  inc rax ; skip null byte
  inc rax ; skip flags
  push qword rax
  mov rdi, rdx
  jmp next

; ( n -- "n")
native ".", dot
  cmp rsp, [data_stack_base]
  jae .empty
  pop rdi
  call print_uint
  mov rdi, SPC
  call print_char
  jmp .done
.empty:
  mov rdi, msg_stack_empty
  call print_string
.done:
  jmp next

native ".S", dotS
  mov rax, rsp
.loop:
  cmp rax, [data_stack_base]
  jae .done
  mov rdi, [rax]  ; print next stack value
  push rax
  call print_int
  mov rdi, SPC
  call print_char
  pop rax
  add rax, CELL_SIZE
  jmp .loop
.done:
  jmp next

; ( name* -- addr/0 )
native "find", find
  mov rdx, rdi
  pop rdi
  push rdx
  push rsi
  mov rsi, [dict_top]
.search:
  ;lea rsi, [rsi + CELL_SIZE]
  add rsi, CELL_SIZE
  push rdi
  push rsi
  call string_equals
  pop rsi
  pop rdi
  cmp rax, 1
  je .done
  mov rsi, [rsi - CELL_SIZE]
  cmp rsi, 0
  je .fail
  jmp .search
.fail:
  pop rsi
  pop rdi
  push qword 0
  jmp next
.done:
  mov rax, rsi
  pop rsi
  pop rdi
  push rax
  jmp next


native "bl", bl
  push SPC
  jmp next

native "+", add
  pop rax
  pop rcx
  add rax, rcx
  push rax
  jmp next

;( n1 n2 -- n1-n2)
native "-", sub
  pop rcx
  pop rax
  sub rax, rcx  ; sub rcx from rax
  push rax
  jmp next

;( n1 n2 -- n1*n2)
native "*", mul
  pop rcx
  pop rax
  xor rdx, rdx
  imul rcx
  push rax
  jmp next

;( n1 n2 -- n1/n2)
native "/", div
  pop rcx
  pop rax
  xor rdx, rdx
  cmp rax, 0
  jge .div
  mov rdx, -1
.div:
  ; rdx:rax/rcx = rax,rdx
  ; up64:low64/div = quot,rem
  idiv rcx
  push rax
  jmp next

native "=", equal
  pop rcx
  pop rdx
  xor rax, rax
  cmp rdx, rcx
  sete al
  push rax
  jmp next

native "<", less
  pop rcx
  pop rdx
  xor rax, rax
  cmp rdx, rcx
  setl al
  push rax
  jmp next

; I ( caddr -- num len) parse num from caddr
; C ( " n " -- n) parse n from stdin
native "number", number
  cmp qword [state], INTERPRET
  je .interpret
  mov rdi, input_buf
  mov rsi, input_buf_size
  call read_word
  call parse_int
  push rax
  jmp next
.interpret:
  mov rdx, rdi
  pop rdi
  push rdx
  call parse_int
  pop rdi
  push rax
  push rdx
  jmp next

; ( c -- "c")
native "emit", emit
  mov r8, rdi
  pop rdi
  push r8
  call print_char
  pop rdi
  jmp next

; ( "c" -- c)
native "key", key
  call read_char
  push rax
  jmp next

native "and", and
  pop rcx
  pop rdx
  xor rax, rax
  test rcx, rdx
  setnz al
  push rax
  jmp next

native "not", not
  pop rcx
  xor rax, rax
  cmp rcx, 0
  setz al
  push rax
  jmp next

; ( a b c -- b c a)
native "rot", rot
  pop rdx
  pop rcx
  pop rax
  push rdx
  push rax
  push rcx
  jmp next
; ( a b -- b a)
native "swap", swap
  pop rdx
  pop rcx
  push rdx
  push rcx
  jmp next
; ( a -- a a)
native "dup", dup
  pop rcx
  push rcx
  push rcx
  jmp next
; ( a -- a a)
native "2dup", 2dup
  pop rcx
  pop rdx
  push rdx
  push rcx
  push rdx
  push rcx
  jmp next
; ( a -- )
native "drop", drop
  add rsp, CELL_SIZE
  jmp next

; ( address u --) stores u starting at address
native "!", store_cell
  pop rcx
  pop rdx
  mov [rdx], rcx
  jmp next
; ( address c --) stores byte at address
native "c!", store_char
  pop rcx
  pop rdx
  mov byte [rdx], cl
  jmp next
; ( address -- u) read cell from address
native "@", read_cell
  pop rdx
  mov rcx, [rdx]
  push rcx
  jmp next
; ( address -- c) read byte from address
native "c@", read_char
  pop rdx
  xor rcx, rcx
  mov byte cl, [rdx]
  push rcx
  jmp next

; compile only
; ( b --) jmp if 0
native "0branch", branch0
  pop rax
  test rax, rax
  jnz .notzero
  mov pc, [pc]
  jmp next
.notzero:
  add pc, CELL_SIZE
  jmp next

; compile only
native "branch", branch
  mov pc, [pc]
  jmp next

; compile only
; ( -- n) push value following this xt
native "lit", lit
  push qword [pc]
  add pc, CELL_SIZE
  jmp next

native "execute", execute
  pop rax
  mov w, rax
  jmp [rax]

; ============
; INTERPRETER
; ============
colon "quit", quit
  dq xt_lit, 1
.eol:
  dq xt_drop
  ;dq xt_lit, LF, xt_emit
  dq xt_lit, prompt, xt_count, xt_type
  dq xt_branch, .loop
.loop:
  dq xt_bl, xt_word ; ( caddr len)
  dq xt_dup, xt_branch0, .eol
  dq xt_drop;, xt_dup  ; ( caddr caddr)
  ;dq xt_read_char, xt_lit, LF, xt_equal, xt_not, xt_branch0, .eol
  dq xt_interpret

  dq xt_branch, .loop
  dq xt_bye

; ( caddr --)
colon "interpret", interpret
  dq xt_dup, xt_find ; ( caddr addr/0 )
  dq xt_dup, xt_branch0, .number
  dq xt_swap, xt_drop
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
  dq xt_dup, xt_read_char, xt_dot,
  dq xt_lit, '"', xt_emit, 
  dq xt_count, xt_type
  dq xt_lit, '"', xt_emit, xt_lit, ']', xt_emit
  dq xt_exit

STATIC_DICT_END EQU DICT_TOP

%endif
