
: loop 
  sdl/fill-rect
  100 sdl/delay
;

variable loopxt

' loop loopxt !

: main 
  sdl/init

  loopxt @ sdl/register-main-loop
; 

main
