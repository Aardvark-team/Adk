&consts:              ; This is need and should be defined

0, String, "Starting fib!"
1, String, "Fib number: "

&consts_end:


; This is actually a "main" function this is just used to know
; the top instruction
; The file is read and executed from the first instruction
; down
main:
  load_const 0
  push 1      ; Pushing arg count
  push 0      ; Pushing syscall function
  syscall

  push 35
  push 1
  call fib(x)

  load_const 1
  push 1
  push 0
  syscall
  pop                 ; Removes junk from syscall. Syscalls always retrun a value

  push 1              ;
  push 0              ;
  syscall             ; output(fib(x))
  halt

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

  getlocal 2          ;
  setlocal 3          ; temp = b

  getlocal 1          ;
  getlocal 2          ;
  const_add           ; a + b
  setlocal 2          ; b = a + b

  getlocal 3          ;
  setlocal 1          ; a = temp

  push 1              ;
  getlocal 4          ;
  const_add           ;
  setlocal 4          ; i++

  getlocal 1          ;
  push 1              ;
  push 0              ;
  syscall             ; output(a)

  jmp fib(x)_loop1    ; Jumps back to the loop

fib(x)_loop1_end:
  getlocal 1          ; Get a
  return              ; return -> returns with first element on the stack