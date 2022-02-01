# Aardvark

This contains all programs for the Aardvark Programming Language. It contains the VM, the Compiler, and the Assembler.

## Aardvark VM


## Aardvark Compiler


## Aardvark Assembler

The Aardvark Assembler is the basic assembler for Aardvark's IR.

### The IR

#### The Constant Pool

The constant pool is how each IR file should start. The constant pool constants all strings and objects.

It should look like this at the top of the file:
```bash
&consts:

0, String, "Hello, World!"    ; Stores "Hello, World!" at index 0 in the pool

&consts_end:
```

#### Instructions

Each instruction is one byte and the operands can be more than one.

- `halt`: 0 operands; Stops the program
- `push`: 1 operand; Pushes a value onto the stack
  - `push 1`, `1` is the value to be pushed
- `pop`: 0 operands; Pops a value of the top of the stack
- `load_const`: 1 operand; Loads a constant on the top of the stack from the constant pool
  - `load_const 0`, Takes the constant at index 0 in the constant pool
- `setlocal`: 1 operand; Sets a local variable by popping the value on the top of the stack.
  - `setlocal 0`, Sets the local 0 with the item on the top of the stack.
  - can be any number but best to use it incrementally
- `getlocal`: 1 operand; Gets a local variable by pushing the value onto the top of the stack
  - `getlocal 0`, Gets the local 0 onto the top of the stack
- `setglobal`: 1 operand; Sets a global variable same as a local.
- `getglobal`: 1 operand; Gets a global variable same as a local.