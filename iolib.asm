global string_length
global print_string
global print_char
global print_newline
global print_uint
global print_int
global string_equals
global read_char
global read_word
global read_word_delim
global parse_uint
global parse_int
global string_copy

%include "stdmac.inc"

section .text

string_length:
  xor rax, rax
.loop:
  cmp byte [rdi+rax], NULL  ; ø terminator
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
  mov rdx, 1
  mov rsi, rsp
  mov rdi, STD_OUT
  mov rax, SYS_WRITE
  syscall
  pop rdi
  ret

print_newline:
  mov rdi, LF
  call print_char
  ret

ASCII_TRANSFORM  EQU 0x30
UINT_MAX_D    EQU  21
; rdi - uint
print_uint:
  push rbp
  push r9
  mov r9, rsp
  mov rax, rdi
  mov rbp, rsp
  mov rbx, rsp
  sub rsp, UINT_MAX_D
  mov byte [rbx], 0
  dec rbx
  
  mov r8, 10
.loop:
  xor rdx, rdx  ; upper 64b
  ;rax contains lower 64b
  div r8    ; rax / r8 . rdx = lowest digit, rax = rax/10
  or dl, ASCII_TRANSFORM
  dec rbx
  mov [rbx], dl
  
;  mov rdi, rdx
;  push rax
;  call print_char
;  pop rax
  test rax, rax  ; finished?
  jnz .loop

  mov rdi, rbx
  call print_string

  mov rsp, r9
  pop r9
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

; rax - character read
read_char:
  mov rdx, 1
  push 0
  mov rsi, rsp
  mov rdi, STD_IN
  mov rax, SYS_READ
  syscall
  pop rax
  ret

; read space delimited word into rdi
; rdi - buffer address
; rsi - buffer length
read_word:
  mov rdx, SPC
  call read_word_delim
  ret

; newlines are treated as delimiters in addition to whatever is specified
; Args:
;   rdi - buffer address
;   rsi - buffer length
;   rdx - word delimiter
; return:
;   rdi - buf
;   rax - word length
read_word_delim:
  push r15
  push r14
  push r13
  mov r15, rsi
  dec r15
  xor r14, r14
  mov r13, rdx
.skip:  ; skip leading delimiters
  push rdi
  call read_char
  pop rdi

  cmp al, r13b
  je .skip
  mov cl, LF
  cmp cl, r13b
  je .skip
.loop:
  cmp r14, r15
  je .toolong

  mov byte [rdi + r14], al
  inc r14

  push rdi
  call read_char
  pop rdi
  cmp al, r13b
  je .delim
  mov cl, LF
  cmp cl, r13b
  je .delim
  jmp .loop

.delim:
  mov byte [rdi+r14], 0
  mov rax, r14
  pop r13
  pop r14
  pop r15
  ret
.toolong:
  xor rax, rax
  pop r13
  pop r14
  pop r15
  ret

; skips leading spaces
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
  movzx r8, byte [rdi]
  cmp r8b, '-'
  jne .positive
  inc rdi
  call parse_uint
  neg rax
  jmp .end
.positive:
  call parse_uint
.end:
  ret 

; rdi - pointer to null-terminated string arg1
; rsi - pointer to target buffer          arg2
; rdx - buffer length
; ret - rax - rdi if fits
;      - 0 if too big
string_copy:
  call string_length
  inc rax        ; null byte
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
