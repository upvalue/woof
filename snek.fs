\ snek.fs - run ./repl prelude.fs snek.fs

: green 0 255 0 255 ;
: black 0 0 0 0 ;

\ place apple at random non-occupied spot
\ snake length is variable
\ occupied tiles is variable of some kind (arrays?)
\ each "tick" grows in the direction (up left down right)
\ if you eat the apple, snek grows
\ that's it...

ray/init

: main 
  begin
    .s
    ray/draw-begin
      black ray/clear-background
    ray/draw-end
    ray/window-close?
  until
;

main
