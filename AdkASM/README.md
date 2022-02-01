# Aardvark Assembler

The Aardvark assembler is a intermediate representaion of the Aardvark VM bytecode. This is so it would be easier to implement a compiler to the bytecode.

## Compiling the IR / Assemebly

To compile your '.adb' program you must use the adkasm binary. This is located in `~/bin`

### Usage
```bash
./bin/adkasm <input file '.adb'> <output file '.adc'>
# Here is an example \/
./bin/adkasm examples/fib.adb fib35.adc
```

This file will then be used to be executed with the AdkVM.
```bash
./bin/adkvm fib35.adc
```