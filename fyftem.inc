%ifndef _FYFTEM_FYFTEM_INC
%define _FYFTEM_FYFTEM_INC

; ===============
; = Definitions =
; ===============
%define L 0
%macro word 3
%%L: dq L   ; pointer to previous word
db %1, 0    ; string representation of word
db %3       ; flags
xt_%2:      ; execution token beginning
%define L %%L
%endmacro

%define DICT_TOP L

%macro native 3
word %1, %2, %3
  dq i_%2    ; native words' execution tokens are pointers to their implementations
i_%2:
%endmacro
%macro native 2
word %1, %2, 0
  dq i_%2
i_%2:
%endmacro

%macro colon 2
word %1, %2, 0
  dq i_docol
%endmacro

%define CELL_SIZE 8

; =============
; = Registers =
; =============
%define pc      r15 ; pointer to next forth command
%define w       r14 ; when non-native word starts execution, points to first word
%define rstack  r13 ; Return stack pointer

; ====================
; = System constants =
; ====================
%define input_buf_size 1024

%endif
