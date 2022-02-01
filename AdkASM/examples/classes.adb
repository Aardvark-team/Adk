&consts:

0, Object, { ~init(), cool() }
3, String, "cool"
2, String, "x"
1, String, "~init()"

&consts_end:

__internal_main__():

  push 0
  new 0
  load_const 1
  invokemethod
  setglobal 0

  getglobal 0
  
  load_const 2
  getfield
  push 1
  push 0
  syscall

  
  
  push 0
  getglobal 0
  load_const 3
  invokemethod
  halt

cool():
  storelcls 0


  getlocal 0
  
  load_const 2
  getfield
  push 1
  push 0
  syscall

  return

~init():
  storelcls 0

  getlocal 0
  
  load_const 2
  push 5
  setfield
  load_const 2
  getfield
  getlocal 0
  return