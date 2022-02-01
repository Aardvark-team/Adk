/**
 * @file common.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief Just some common definitions
 * @version 0.1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <exception>
#include <sstream>
#include <iomanip>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

// The max stack limit 512 possible values on the stack
#define STACK_LIMIT 512

namespace Aardvark {
  using std::string;
  using std::vector;
  using std::map;
  using std::unordered_map;
  using std::ostringstream;
};

#endif