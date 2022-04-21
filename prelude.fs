\ prelude.fs - basic forth defined functionality

\ some shorthand for VM constants to make this a bit more readable

: OP_JUMP_IF_ZERO 4 ; 
: OP_JUMP 5 ; 

\ \\\\ CONDITIONALS

: if 
  \ emit jump instruction
  OP_JUMP_IF_ZERO ,
  \ save address of jump address onto stack for later overwriting
  here
  \ emit -1 as placeholder for address
  -1 , 
; immediate compile-only


: else 
  \ emit jump unconditional instruction
  OP_JUMP ,
  \ save address of jump address onto stack for later overwriting
  here
  \ emit -1 as a placeholder for address
  -1 , 
  \ swap address of this jump address with the one placed onto the stack by if
  swap
  \ overwrite previous jump address with here
  here swap !
; immediate compile-only
  
: then 
  \ overwrite jump address with address of this code
  here swap !
; immediate compile-only

\ \\\\ LOOPS

\ begin - start a loop
: begin here ; immediate compile-only

\ again - endless loop
: again
  \ emit jump instruction
  OP_JUMP , 
  \ jump back to loop beginning
  ,
; immediate compile-only

: until 
  OP_JUMP_IF_ZERO ,
  \ jump back to loop beginning if zero
  ,
; immediate compile-only

\ \\\\\ INPUT/OUTPUT

: cr "\n" fmt ;

\ \\\\\ ARRAYS

\ \\\\\ STACK MANIPULATION

: dup2 { a b } a b a b ;
