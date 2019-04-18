%ifndef _FYFTEM_DICTIONARY_INC
%define _FYFTEM_DICTIONARY_INC
%include "fyftem.inc"
%include "iolib.inc"

section .text
; ============
; dictionary
; ============
first_word:
; ( a -- )
native "drop", drop
	add rsp, 8
	jmp next

; initialises registers
native "init", init
	mov rstack, rstack_start
	mov pc, main_stub
	jmp next

; saves pc at start of colon word
native "docol", docol
	sub rstack, 8
	mov [rstack], pc
	add w, 8
	mov pc, w
	jmp next

; returns from colon word
native "exit", exit
	mov pc, [rstack]
	add rstack, 8
	jmp next

; ( buf* -- )
; read word from stdin, store in buf
native "word", word
	pop rdi
	call read_word
	push rdx		; WHAT'S GOING ON HERE?
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
	lea rdi, [rdi + 8]
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
	lea rsi, [rsi + 8]
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

native ".", dot
	pop rdi
	call print_uint
	jmp next

; colon word :. stores execution tokens
colon "main", main
	dq xt_inbuf
	dq xt_word
; dq xt_drop
	dq xt_inbuf
	dq xt_find_word
	dq xt_cfa
	dq xt_dot
	dq xt_inbuf
	dq xt_prints
	dq xt_bye

%endif
