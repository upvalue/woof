\ prelude.fs - basic forth defined functionality

: if 
  \ emit jump instruction
  4 ,
  \ save address of jump address onto stack for later overwriting
  here
  \ emit -1 as placeholder for address
  -1 , 
; immediate compile-only


: else 
  \ emit jump unconditional instruction
  5 ,
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

\ would be cool to do this in forth

\ ideally you could compose these features together, locals and loops
\ user could optionally specify locals to name loop variables, if they want
\ to use them in the loop :thinkies:
: do 
  \ limit start do { limit start } 
  \ emit code to save variable on stack (start) as local
  \ >l 
  \ emit code to save variable on stack (limit) as local
  \ >l 

  \ save local indexes onto stack and increment the shared
  \ counter, whatever that is

  \ save address of here onto stack (beginning of loop)
  555
; immediate compile-only

: loop 
  \ stack 
  \ ( limit-index-local iterator-local begin-label )
  \ swap rot 
  \ ( begin-label iterator-local limit-index-local )
  \ emit code to check limit-index-local against iterator-local
  \ = 
  \ jump-if-not-zero to end
  \   save addr for end on stack
  \ jump to beginning
  \   overwrite addr for end on stack with after this jump
  \ execution continues!
; immediate compile-only

\ limit start do? loop
\ limit = save as limit
\ start = save as start
\ loop = check start against limit
\ if start < limit
\   jump past end of loop
\ else
\   jump to start of loop
\ then 

\ do saves these as local variables 
\ saves variable
\ loop picks that up and emits code to check them, somehow
\ jmpz 

\ maybe in order to do this, i need a concept of shared variables
\ then local count is consistent within an entire word, and i manipulate
\ local count to figure out what to emit

\ : do? 
  

\ do? emits code for local-set of both variables
\ pushes local idxes onto stack
\ loop picks up local idxes and uses it to finish loop
\ do? push local-set
