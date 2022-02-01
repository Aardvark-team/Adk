#include "../vm.h"

using namespace Aardvark;

// These built-in modules must also be defined within the compiler
// They need to be defined there so the compiler knows they exist and won't throw an error
// To do this go into the compiler's header/include files and edit reservedWords.h
// and add to the modules vector the name of the module
// This name will be what you have to type inside the include
// eg: #include modulename

// This is the structure of a NativeFunction to be used with Aardvark
/**
 * @param argCount - the number of arguments passed from the caller
 * @param args - The number of arguments this will be the size of argCount
 * @param self - The self variable is used if the function was called within an object otherwise it is nullptr
 *
 * @return AdkValue - Should always return an AdkValue even if it is an empty AdkValue
 */
AdkValue printHelloWorld(int argCount, AdkValue* args, AdkObject* self) {
  std::cout << "Hello, World!" << std::endl;

  if (argCount > 0) {
    std::cout << args[0].toString() << std::endl;
  }

  return AdkValue();
}

// Entry point. Can be any name just has to be called within the main function in main.cpp
// It must return an objPtr / AdkValue. The returned value should always be an object
AdkValue initTestModule(VM* vm) {
  // First param is object name or classname. Second is to tell it to create a standard object
  // Second param is there so if you want to create a string you set it to false
  // It will then use the first param to create a String with that AdkValue
  
  AdkValue objPtr = vm->createObject("HelloWorld", true);
  objPtr.as.obj->methods["printHW"] = Method{
    0, // addr of method only for Aardvark methods
    "printHW", // name of method can be anything really
    printHelloWorld // the native function
  };
  
  return objPtr;
}