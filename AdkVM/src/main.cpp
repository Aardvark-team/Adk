#define LOGINSTR(value) (std::cout << #value << " = 0x" << std::setfill('0') << std::setw(2) << std::hex << (int)value << "\n")
#include "../include/vm.h"
#include "modules/testExports.hpp"

using namespace Aardvark;

AdkValue output(int argCount, AdkValue* args, AdkObject* self) {
  for (int i = 0; i < argCount; i++) {
    if (i > 0) {
      std::cout << " ";
    }
    AdkValue arg = args[i];

    if (arg.type == DataTypes::Object) {
      string objType = arg.as.obj->objType;

      if (objType == "String") {
        AdkString* str = (AdkString*)arg.as.obj;
        std::cout << str->toString();
      }
    } else if (arg.type == DataTypes::Int || arg.type == DataTypes::Double) {
      std:: cout << arg.as.number;
    }
  }

  std::cout << "\n";
  return AdkValue();
}

AdkValue input(int argCount, AdkValue* args, AdkObject* self) {
  for (int i = 0; i < argCount; i++) {
    if (i > 0) {
      std::cout << " ";
    }
    AdkValue arg = args[i];

    if (arg.type == DataTypes::Object) {
      string objType = arg.as.obj->objType;

      if (objType == "String") {
        AdkString* str = (AdkString*)arg.as.obj;
        std::cout << str->toString();
      }
    } else if (arg.type == DataTypes::Int || arg.type == DataTypes::Double) {
      std:: cout << arg.as.number;
    }
  }

  string value;
  std::cin >> value;

  return AdkValue(value);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: adkvm <file>" << std::endl;
    return 1;
  }
  VM vm = VM();
  vm.define(0, output); // A defined syscall
  vm.define(1, input); // A defined syscall

  vm.modules["hw"] = AdkModule{
    "hw", // module path or name if its a C++ module
    initTestModule
  };

  Program prog2;

  vm.readProgramFile(argv[1], &prog2);
  vm.run();

  return 0;
}