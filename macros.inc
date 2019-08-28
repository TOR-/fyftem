%ifndef _FYFTEM_macros.inc
%define _FYFTEM_macros.inc

; ===============
; = Definitions =
; ===============
%define L 0
%macro word 3
l%%L: dq L   ; pointer to previous word
db %1, 0    ; string representation of word
db %3       ; flags
xt_%2:      ; execution token beginning
%define L l%%L
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

%endif
