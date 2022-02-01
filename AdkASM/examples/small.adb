&consts:

0, Object, {     ; An object currently is just a list of methods
  test
}
1, String, "Hello, World!"

&consts_end:

main:
  load_const 1   ; Load arguments to method call
  push 1
  new 0          ; Create a new object copy from the constant pool
  push 0         ; The method index of the object. In this case it is 'test'
  invokemethod   ; invokes a method on an object
  halt
test:
  storelcls 1    ; Since this is a method, local 0 is the 'this' pointer
  getlocal 1     ; All other args are stored after
  push 1
  push 0
  syscall        ; Syscall for 'output'
  return

; This is equivalent to:
; class Test {
;   funct test(x) {
;     output(x)
;   }
; }
;
; Test().test("Hello, World!")
;