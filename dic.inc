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
; ( a -- )
native "drop", drop
	add rsp, 8
	jmp next

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
	lea rdi, [rdi + 8]
	push rdi
	call string_length
	pop rdi
	add rax, rdi
	inc rax
	push qword rax
	jmp next

native ".S", dotS
	mov rax, rsp
.loop:
	cmp rax, [data_stack_base]
	jae .done
	mov rdi, [rax]	; print next stack value
	push rax
	call print_uint
	mov rdi, SPC
	call print_char
	pop rax
	add rax, 8
	jmp .loop
.done:
	jmp next

native ".", dot
	pop rdi
	call print_uint
	jmp next

native "bl", bl
	push SPC
	jmp next

; colon word : stores execution tokens

%endif
