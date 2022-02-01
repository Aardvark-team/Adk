/**
 * @file AdkString.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief A string class for pushing into the stack
 * @version 0.1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef ADKSTRING_H
#define ADKSTRING_H
#include "../common.h"
#include "../datatypes.h"
#include "AdkValue.h"
#include "AdkObject.h"

namespace Aardvark {

  class AdkString : public AdkObject {
    public: 
    AdkString(): AdkObject("String") {};
    AdkString(string value): AdkObject("String") {
      ConstructString(value);
    };

    void ConstructString(string str) {
      int length = str.size();

      props["length"] = AdkValue(length);

      for (int i = 0; i < length; i++) {
        props[std::to_string(i)] = AdkValue(str[i]);
      }
    };

    string toString() {
      string str = "";
      int length = (int)props["length"].as.number;

      for (int j = 0; j < length; j++) {
        str += (char)((int)props[std::to_string(j)].as.number);
      }

      return str;
    }
  };

}; // namespace Aardvark

#endif