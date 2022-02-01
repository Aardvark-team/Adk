# Aardvark Virtual Machine

This is a virtual machine for the programming language Aardvark. There will be an IR of the bytecode that may also be ran.

## Running a Program

### In Replit
Copy the binary located in /bin. It should be called adkvm. Place that file into a repl. Run the program with 1 argument of the compiled aardvark file.
```bash
# This will run the program fib35
./bin/adkvm fib35.adc
```

### Everywhere else
Copy the binary into a folder. Setup the environment variable 'PATH' to the path of the folder where the binary is in. Reload any terminals, command prompts, shells, or powershells and run a program like so:
```bash
# This will run the program fib35
adkvm fib35.adc
```

**Note:** currently absolute paths only work when using the 'PATH' it does not check the CWD to find the file **NOT IMPL!**

## The Bytecode

Stuff here

## Intermediate Representation

In the intermediate representation you would build a program as follows:

Constants always need to be defined at the top of the file
Constants are stored by the index and type
```c
&consts:

0, String, "Hello, World!" ; Storing a constant

&consts_end:
```

All the actual 'code' will be after the `&consts_end`
```c
&consts:

0, String, "Hello, World!" ; Storing a constant

&consts_end:

main:
  load_const 0    ; Loads the 0th constant or the constant at index 0 on the stack
  setlocal 0      ; Sets the local at index 0 via whatever is on the top of stack

  getlocal 0      ; Gets the local and pushes its value onto the stack
  push 1          ; The number of arguments for the function call
  push 0          ; The number of syscall
  syscall         ; Execute the syscall

  ; syscall 0 is 'output'

  halt            ; Stop the program
```

## The .adc files

.adc file are the files that would be the compiled bytecode with some extra data for constants and other things.

The first eight bits represent the files magic number.
The .adc magic number is `11011101`.

### Constant Pool
The next 8 bits is the constant pools size.
*eg:* `00000001` - this is a size of 1
This will then look like `1101110100000001` at the beginning of the file

#### Definitions

The next eight bits are the index into the constant pool. This will be repeated for each constant pool definition.
The next 16 bits are the size of the current definition.
*eg:*
`0000 0000000000001100`
`01010011011101000110000101110010011101000110100101101110011001110010000001100110011010010110001000100001`
**This will be the string "Starting Fib!" at constant pool index 0**

This segment will be repeated depending on the constant pool size. *eg:* if the size was 3 then the section will be repeated 3 times.

Everything after that would be the Aardvark bytecode.