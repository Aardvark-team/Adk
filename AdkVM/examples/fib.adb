&consts:

0, String, "Starting fib!"

&conts_end:


fib(x):
  storelcls 1         ;  Gets argcount off stack; pops them and stores them into locals from 0...n. Only stores for the defined amount of args
  push 0
  setlocal 1          ; Variable a
  
  push 1
  setlocal 2          ; Variable b

  push 0
  setlocal 3          ; Variable temp

  push 0
  setlocal 4          ; Variable i

fib(x)_loop1:
  getlocal 0          ; Get x
  getlocal 4          ; Get i
  cmp_lt              ; i < x
  jmp_nt fib(x)_loop1_end       ; Jumps if not true, to the end of the loop

  getlocal 2
  setlocal 3          ; temp = b

  getlocal 1
  getlocal 2
  const_add           ; a + b
  setlocal 2          ; b = a + b

  getlocal 3
  setlocal 1          ; a = temp

  jmp fib(x)_loop1    ; Jumps back to the loop

fib(x)_loop1_end:
  getlocal 1          ; Get a
  return              ; return -> returns with first element on the stack