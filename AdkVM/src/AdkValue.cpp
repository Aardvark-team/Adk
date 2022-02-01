#include "../include/values/AdkValue.h"
#include "../include/values/AdkObject.h"
#include "../include/values/AdkString.h"

namespace Aardvark
{
  
  AdkValue::AdkValue(string str): type(DataTypes::Object) {
    as.obj = (AdkObject*)new AdkString(str);
  }

  bool AdkValue::isObject() {
    return type == DataTypes::Object;
  }

  bool AdkValue::isString() {
    return (
      (
        isObject()
      )
      &&
      (
        as.obj->objType == "String"
      )
    );
  }

  AdkString* AdkValue::toAdkString() {
    if (!isString()) {
      std::cerr << "Error: Cannot convert to AdkString as value is not a string!\n";
      exit(1);
    }

    return (AdkString*)as.obj;
  }

  string AdkValue::toString() {

    if (isString()) {
      AdkString* strObj = toAdkString();
      return strObj->toString();
    } else if (isObject()) {
      return "[Object object]";
    }

    std::ostringstream str;
    str << as.number;
    return str.str();
  }

}; // namespace Aardvark
