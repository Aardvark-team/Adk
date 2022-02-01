/**
 * @file AdkValue.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief The values to be stored in the stack
 * @version 0.1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef ADKVALUE_H
#define ADKVALUE_H
#include "../common.h"
#include "../datatypes.h"

namespace Aardvark {
  class AdkObject;
  class AdkString;

  class AdkValue {
    public:
    DataTypes type;

    union {
      double number;
      int integer;
      char character;
      AdkObject* obj;
      void* ptr;
    } as;

    AdkValue(): type(DataTypes::None) {};
    AdkValue(DataTypes type): type(type) {};
    AdkValue(int integer): type(DataTypes::Int) {
      as.number = (double)integer;
    }
    AdkValue(double dbl): type(DataTypes::Double) {
      as.number = dbl;
    }

    AdkValue(string str); // Constructs a ptr to a string

    bool isObject();
    bool isString();
    string toString();
    AdkString* toAdkString();
  };

}; // namespace Aardvark

#endif