\ fizzbuzz.fs - fizzbuzz

: fizzbuzz-print
  dup "%d" fmt
  dup 3 % 0 = if
    "Fizz" fmt
  then
  dup 5 % 0 = if
    "Buzz" fmt
  then
  cr
;

: fizzbuzz { limit } 
  0 
  begin
    fizzbuzz-print
    1 + 
    dup limit = 
  until
  drop
;
    
101 fizzbuzz
