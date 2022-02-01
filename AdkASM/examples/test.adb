&consts:

0, String, "Test"
1, String, "Hello"
2, String, "cool"
3, String, "World!"

&consts_end:

__internal_main__():
  load_const 0
  setlocal 0
  load_const 1
  push 1
  push 0
  syscall

  push 4
  push 5
  push 2
  call test(x,y)


  getlocal 0
  push 1
  push 0
  syscall
  load_const 2
  setlocal 0

  getlocal 0
  push 1
  push 0
  syscall

  halt

test(x,y):
  storelcls 2

  load_const 3
  push 1
  push 0
  syscall


  getlocal 0
  push 1
  push 0
  syscall


  getlocal 1
  push 1
  push 0
  syscall

  return