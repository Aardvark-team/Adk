/**
 * @file opcodes.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief Opcodes of the Aardvark VM
 * @version 0.1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef OPCODES_H
#define OPCODES_H

enum {
  HALT, // 0

  CONST_ADD, // 1
  CONST_SUB, // 2
  CONST_MUL, // 3
  CONST_DIV, // 4
  CONST_MDO, // Modulo Aka ('%')

  LD_CONST,  // 6

  PUSH,      // 7
  POP,       // 8

  SYSCALL,   // 9

  SETGLBL,   // 10
  GETGLBL,   // 11
  SETLCL,    // 12
  GETLCL,    // 13

  STRELCLS,  // 14

  CALL,      // 15
  INVOKEMETHOD, // 16
  RETURN,    // 17
  RETURNVOID,// 18

  CMP_EQ,    // 19
  CMP_LT,    // 20
  CMP_LTE,   // 21
  CMP_GT,    // 22
  CMP_GTE,   // 23

  JMP,       // 24
  JMP_T,     // 25
  JMP_NT,    // 26

  NEW,       // 27
  SETFIELD,  // 28
  GETFIELD,  // 29

  DUPE,      // 30
  SWAP,      // 31

  IMPORT     // 32
};

#endif