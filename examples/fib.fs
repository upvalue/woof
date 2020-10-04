\ fib.fs - prints fibonacci numbers

: fib ( n -- n )
   dup 2 < if
   else
     dup 1 - fib
     swap
     2 - fib 
     +
   fi
;

30 fib .
