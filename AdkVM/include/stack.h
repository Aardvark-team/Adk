/**
 * @file stack.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief The main definitions of the virtual machine's stack
 * @version 0.1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef STACK_H
#define STACK_H
#include "common.h"
#include "values/AdkValue.h"
#include "values/AdkObject.h"
#include "values/AdkString.h"

namespace Aardvark {

  class Stack {
    public:
    // The actual stack
    AdkValue data[STACK_LIMIT];
    int size = 0;

    private:
  };

}; // namespace Aardvark

#endif