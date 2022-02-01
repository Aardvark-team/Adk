/**
 * @file AdkObject.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief A object class to represent other objects. Other values may inherit this
 * @version 0.1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef ADKOBJECT_H
#define ADKOBJECT_H
#include <functional>
#include "../common.h"
#include "../datatypes.h"
#include "AdkValue.h"

namespace Aardvark {
  class AdkObject;
  
  typedef std::function<AdkValue(int argCount, AdkValue* args, AdkObject* self)> NativeFunction;

  struct Method {
    uint16_t addr = 0;
    string name = "__NATIVE_FUNC__";
    NativeFunction func = nullptr;
  };
  class AdkObject {
    public:
    DataTypes type = DataTypes::Object;
    string objType = "None";

    unordered_map<string, AdkValue> props;
    unordered_map<string, Method> methods;

    AdkObject* next = nullptr; // Linked list for garbage collection and cleaning any allocated memory

    AdkObject() {};
    AdkObject(string type): objType(type) {};

    bool hasMethod(string methodName) {
      return methods.find(methodName) != methods.end();
    }

    bool hasProp(string propName) {
      return props.find(propName) != props.end();
    }
  };

}; // namespace Aardvark

#endif