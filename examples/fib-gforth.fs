\ fib.fs - fibonacci numbers

: fib \ n -- n
  \ if this number is < 2, it's already the correct fibonacci
  \ number, so just return
  dup 1 > if
    \ duplicate the value for multiple calls
    dup
    \ call fib against n - 1
    1 - recurse
    \ swap to n back on top
    swap
    \ call fib against n - 2
    2 - recurse
    \ add results of both
    +
  then
;

35 fib .
