/**
 * @file vm.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief The main definitions of the virtual machine classes / code
 * @version 0.1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef VM_H
#define VM_H
#include <functional>
#include <fstream>
#include "common.h"
#include "stack.h"
#include "opcodes.h"
#include "program.h"

namespace Aardvark {
  class VM;
  typedef std::function<AdkValue(int argCount, AdkValue* args, AdkObject* self)> NativeFunction;
  typedef std::function<AdkValue(VM* vm)> NativeModule;

  struct VMState {
    Stack stack;
    AdkValue* constants;
    size_t constantSize;

    std::vector<AdkValue> globals;
    std::vector<AdkValue*> localList;

    AdkValue* locals;

    u8* ip;
    std::vector<u8*> rp; // return pointer
    AdkValue* sp;
  };

  class AdkModule {
    public:
    string modulePath;

    NativeModule entry = nullptr;
  };

  class VM {
    public:
    u8* code;
    
    Stack stack;
    AdkValue* constants;
    size_t constantSize;

    std::vector<AdkValue> globals;
    std::vector<AdkValue*> localList;
    AdkValue* locals;

    vector<NativeFunction> syscalls;
    unordered_map<string, AdkModule> modules;

    AdkObject* objs;

    u8* ip;
    std::vector<u8*> rp; // return pointer
    AdkValue* sp;

    VM();

    bool hasModule(string moduleName);

    void freeObject(AdkObject* obj);
    void freeObjects();
    AdkValue createObject(std::string x, bool isType);
    AdkValue copyObject(AdkObject* obj);

    void clean();

    VMState saveState();
    void loadState(VMState state);
    void appendState(VMState state);

    void createLocals();
    void pushLocals();
    void popLocals();



    void setLocal(int index, AdkValue val);
    AdkValue getLocal(int index);

    void setGlobal(int index, AdkValue val);
    AdkValue getGlobal(int index);

    void push(AdkValue val);
    AdkValue pop();

    AdkValue run(u8* code);
    AdkValue run();
    void define(size_t sysNum, NativeFunction func);

    void printStack();

    void writeProgramToFile(std::string filename, Program prog);
    void readProgramFile(std::string filename, Program* prog);

    private:
  };

}; // namespace Aardvark

#endif