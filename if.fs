: if 
  \ emit jump instruction
  4 ,
  \ save address of jump address onto stack for later overwriting
  here
  \ emit 555 as placeholder for address
  555 , 
; immediate


: else 
  \ emit jump unconditional instruction
  5 ,
  \ save address of jump address onto stack for later overwriting
  here
  \ emit 555 as a placeholder for address
  555 , 
  \ swap address of this jump address with the one placed onto the stack by if
  swap
  \ overwrite previous jump address with here
  here swap !
; immediate
  
\ TODO else

: then 
  \ overwrite jump address with address of this code
  here swap !
; immediate

