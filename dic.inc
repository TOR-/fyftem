%ifndef _FYFTEM_DICTIONARY_INC
%define _FYFTEM_DICTIONARY_INC
%include "fyftem.inc"
%include "iolib.inc"

section .data
data_stack_base: ; store data stack start address
	dq 0

section .text
; ============
; dictionary
; ============
first_word:
; initialises registers
native "init", init
	mov rstack, rstack_start		; reset return stack
	mov pc, main_stub						; load entry point
	cmp qword [data_stack_base], 0 ; initialised yet?
	je .uninitialised
	mov rsp, [data_stack_base]  ; reset data stack
	jmp next
.uninitialised:
	mov [data_stack_base], rsp
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

; ( char "<char>*ccc<char>" -- c-addr )
; skip leading <char>s
; read ccc into buffer
; c-addr = buffer*
native "word", word
	pop rdx
	mov rdi, input_buf
	mov rsi, input_buf_size
	push rdi
	call read_word_delim
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
native "prints", prints
	pop rdi
	call print_string
	jmp next

; exits program
native "bye", bye
	mov rax, 60
	xor rdi, rdi
	syscall

; ( -- @input_buf )
native "inbuf", inbuf
	push qword input_buf
	jmp next

; ( name -- codeaddr )
native "cfa", cfa
	pop rdi
	lea rdi, [rdi + CELL_SIZE]
	push rdi
	call string_length
	pop rdi
	add rax, rdi
	inc rax
	push qword rax
	jmp next

; ( name* -- addr/0 )
native "find_word", find_word
	pop rdi
	mov rsi, DICT_TOP
.search:
	lea rsi, [rsi + CELL_SIZE]
	push rdi
	push rsi
	call string_equals
	pop rsi
	pop rdi
	cmp rax, 1
	je .done
	mov rsi, [rsi]
	cmp rsi, 0
	je .fail
	jmp .search
.fail:
	push qword 0
	jmp next
.done:
	push qword rsi
	jmp next

native ".S", dotS
	mov rax, rsp
.loop:
	cmp rax, [data_stack_base]
	jae .done
	mov rdi, [rax]	; print next stack value
	push rax
	call print_int
	mov rdi, SPC
	call print_char
	pop rax
	add rax, CELL_SIZE
	jmp .loop
.done:
	jmp next

; ( n -- "n")
native ".", dot
	pop rdi
	call print_uint
	mov rdi, SPC
	call print_char
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
	sub rax, rcx	; sub rcx from rax
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

; ( " n " -- n)
native "number", number
	mov rdi, input_buf
	mov rsi, input_buf_size
	call read_word
	call parse_int
	push rax
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
; ( a -- )
native "drop", drop
	add rsp, CELL_SIZE
	jmp next

%endif
