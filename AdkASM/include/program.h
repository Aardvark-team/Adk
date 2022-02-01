#ifndef PROGRAM_H
#define PROGRAM_H

#include <iostream>
#include <cstdint>
#include <vector>

#define MAGICNUMBER 0b11011101
#define VERSION 0b00000100

namespace Aardvark
{
  struct Constant {
    uint8_t type;
    uint16_t size;
    uint8_t* constant;
  };

  struct Program {
    std::vector<Constant> constants;
    std::vector<uint8_t> instructions;
  };
}; // namespace Aardvark

#endif