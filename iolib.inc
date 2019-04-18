%ifndef _FYFTEM_IOLIB_INC
%define _FYFTEM_IOLIB_INC
%include "stdmac.inc"
;section .data
;uint_str:		db "0", 0

section .text
;global _start
;_start:
;	mov rdi, uint_str
;	push rdi
;	call print_string
;	pop rdi
;	call parse_uint
;	push rax
;	push rdx
;	call print_newline
;	pop rdx
;	pop rax
;	mov rdi, rax		; uint value
;	call print_uint
;	mov rdi, rdx		; uint # of digits
;	call print_uint
	
;	mov rdi, rdx
;	mov rax, SYS_EXIT
;	syscall

; rdi - pointer to null-terminated string
; ret - rax length of string
string_length:	
    xor rax, rax
.loop:
	cmp byte [rdi+rax], NULL	; ø terminator
	je .exit
	inc rax
	jmp .loop
.exit:
    ret

; rdi - pointer to null-terminated string
print_string:
	call string_length
	mov rdx, rax
	mov rsi, rdi
	mov rdi, STD_OUT
	mov rax, SYS_WRITE
	syscall
    ret

; rdi - character code
print_char:
	push rdi
	mov rdi, rsp
	call print_string
	pop rdi
    ret

print_newline:
    mov rdi, LF
	call print_char
    ret

ASCII_TRANSFORM	EQU 0x30
UINT_MAX_D		EQU	21
; rdi - uint
print_uint:
	push rbp
	push rsp
	mov rax, rdi
	mov rbp, rsp
	sub rsp, UINT_MAX_D
	mov byte [rbp], 0
	dec rbp
	
	mov r8, 10
.loop:
	xor rdx, rdx	; upper 64b
	;rax contains lower 64b
	div r8		; rax / r8 . rdx = lowest digit, rax = rax/10
	or dl, ASCII_TRANSFORM
	dec rbp
	mov [rbp], dl
	test rax, rax	; finished?
	jnz .loop

	mov rdi, rbp
	call print_string

;	add rsp, 24

	pop rsp
	pop rbp
	ret

print_int:
    test rdi, rdi
    jns print_uint
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    neg rdi
    jmp print_uint
    ret

string_equals:
    mov al, byte [rdi]
    cmp al, byte [rsi]
    jne .no
    inc rdi
    inc rsi
    test al, al
    jnz string_equals
    mov rax, 1
    ret
    .no:
    xor rax, rax
    ret 


read_char:
	mov rdx, 1
	push 0
	mov rsi, rsp
	mov rdi, STD_IN
	mov rax, SYS_READ
	syscall
	pop rax
    ret

; Just here to satisfy test.py
read_word:
    push r14
    push r15
    xor r14, r14 
    mov r15, rsi
    dec r15

.A:
    push rdi
    call read_char
    pop rdi
    cmp al, ' '
    je .A
    cmp al, 10
    je .A
    cmp al, 13
    je .A 
    cmp al, 9 
    je .A
    test al, al
    jz .C

.B:
    mov byte [rdi + r14], al
    inc r14

    push rdi
    call read_char
    pop rdi
    cmp al, ' '
    je .C
    cmp al, 10
    je .C
    cmp al, 13
    je .C 
    cmp al, 9
    je .C
    test al, al
    jz .C
    cmp r14, r15
    je .D

    jmp .B

.C:
    mov byte [rdi + r14], 0
    mov rax, rdi 
   
    mov rdx, r14 
    pop r15
    pop r14
    ret

.D:
    xor rax, rax
    pop r15
    pop r14
    ret

; This works, but not with test.py
; rdi - buffer address
; rsi - buffer length
;read_word:
;	xor r8, r8
;.loop:
;	cmp r8, rsi
;	je .toolong
;
;	push r8
;	push rdi
;	push rsi
;	call read_char
;	pop rsi
;	pop rdi
;	pop r8
;
;	cmp al, SPC
;	je .ws
;	cmp al, TAB
;	je .ws
;	cmp al, LF
;	je .ws
;	mov [rdi + r8], al
;	inc r8
;	jmp .loop
;
;.ws:
;	mov byte [rdi+r8], 0
;	mov rax, rdi
;	ret
;.toolong:
;	mov rax, 0
;	ret

; rdi points to a string
; returns rax: number, rdx : length
; pushes each number as it's converted
; pops and adds to acc at end
;parse_uint:
;	xor rax, rax
;	xor rdx, rdx
;	mov r10, 10
;.loop:
;	xor rax, rax
;	movzx ax, byte [rdi + rdx]
;	cmp al, '0'
;	jb .exit
;	cmp al, '9'
;	ja .exit
	
;	push rax
;	push rdx
;	mov rax, r9
;	mul r10
;	mov r9, rax
;	pop rdx
;	pop rax
	
;	sub al, ASCII_OFFS
;	rax -- digit
;	add r9, rax
;	inc rdx
;	jmp .loop

;.exit:
;	mov rax, r9
;    ret

; returns rax: number, rdx : length
parse_uint:
    mov r8, 10
    xor rax, rax
    xor rcx, rcx
.loop:
    movzx r9, byte [rdi + rcx] 
    cmp r9b, '0'
    jb .end
    cmp r9b, '9'
    ja .end
    xor rdx, rdx 
    mul r8
    and r9b, 0x0f
    add rax, r9
    inc rcx 
    jmp .loop 
    .end:
    mov rdx, rcx
    ret

; rdi points to a string
; returns rax: number, rdx : length
parse_int:
    xor rax, rax
    ret 

; rdi - pointer to null-terminated string	arg1
; rsi - pointer to target buffer			arg2
; rdx - buffer length
; ret - rax - rdi if fits
;			- 0 if too big
string_copy:
	call string_length
	inc rax				; null byte
	cmp rax, rdx
	jg .toobig
	xor r8, r8
.move:
	cmp qword r8, rax
	jge .justright
	mov r9, [rdi+r8]
	mov [rsi+r8], r9
	inc r8
	jmp .move
.toobig:
	mov qword rax, 0
	ret
.justright:
	mov rax, rsi
	ret

%endif