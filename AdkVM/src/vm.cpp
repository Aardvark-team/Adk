#include "../include/vm.h"
#include "../include/errors.h"

// Some useful macros
#define READ_BYTE() (*ip++)
#define LOGINSTR(value) (std::cout << #value << " = 0x" << std::setfill('0') << std::setw(2) << std::hex << (int)value << "\n")
#define LOG(value) (std::cout << #value << " = " << value.as.number << std::endl)

namespace Aardvark
{

  VM::VM() {
    stack = Stack();
    ip = &code[0];
    sp = &stack.data[0];
  };

  bool VM::hasModule(string moduleName) {
    return modules.find(moduleName) != modules.end();
  }

  void VM::freeObject(AdkObject* obj) {
    delete obj;
  }

  void VM::freeObjects() {
    AdkObject* obj = objs;

    while (obj != nullptr) {
      AdkObject* next = obj->next;
      freeObject(obj);
      obj = next;
    }
  }

  // First param is object name or classname. Second is to tell it to create a standard object
  // Second param is there so if you want to create a string you set it to false
  // It will then use the first param to create a String with that AdkValue
  AdkValue VM::createObject(std::string data, bool isType = false) {
    AdkObject* obj = isType ? new AdkObject(data) : new AdkString(data);
    AdkValue value = AdkValue(DataTypes::Object);
    value.as.obj = obj;

    obj->next = objs;
    objs = obj;

    return value;
  }

  AdkValue VM::copyObject(AdkObject* obj) {
    AdkObject* newObj = new AdkObject(obj->objType);
    newObj->props = obj->props;
    newObj->methods = obj->methods;
    newObj->next = objs;
    objs = newObj;

    AdkValue newObjPtr = AdkValue(DataTypes::Object);
    newObjPtr.as.obj = newObj;

    return newObjPtr;
  }

  void VM::clean() {
    freeObjects();

    delete constants;
    delete code;
  }

  void VM::printStack() {
    std::cout << "STACK:\n\n";
    for (auto val : stack.data) {
      std::cout << "[" << val.as.number << "], ";
    }
    std::cout << "ENDSTACK\n\n";
  }

  VMState VM::saveState() {
    return VMState{
      stack,
      constants,
      constantSize,

      globals,
      localList,

      locals,

      ip,
      rp, // return pointer
      sp
    };
  }

  void VM::loadState(VMState state) {
    stack = state.stack;
    constants = state.constants;
    constantSize = state.constantSize;

    globals = state.globals;

    localList = state.localList;
    locals = state.locals;

    ip = state.ip;
    rp = state.rp;
    sp = state.sp;
  }

  void VM::appendState(VMState state) {
    AdkValue* newLocals = new AdkValue[150];
    AdkValue* newConstants = new AdkValue[constantSize + state.constantSize];

    for (int i = 0; i <= 150; i++) {
      if (i / 75 < 1) {
        newLocals[i] = locals[i];
      } else {
        newLocals[i] = state.locals[i - 75];
      }
    }

    for (size_t i = 0; i < constantSize; i++) {
      newConstants[i] = constants[i];
    }

    for (size_t i = 0; i < state.constantSize; i++) {
      newConstants[i + constantSize] = state.constants[i];
    }

    constantSize += state.constantSize;

    globals.insert(globals.end(), state.globals.begin(), state.globals.end());

    locals = newLocals;
    constants = newConstants;
  }

  void VM::createLocals() {
    locals = new AdkValue[75]; // maybe auto determine in assembler and use some kind of instr idk. For now just 75 statically
  }

  void VM::pushLocals() {
    // AdkValue size = AdkValue(75);
    localList.push_back(locals);
  }

  void VM::popLocals() {
    delete locals;
    AdkValue* arrptr = localList[localList.size() - 1];
    localList.pop_back();

    locals = arrptr;
  }

  void VM::setLocal(int index, AdkValue val) {
    if (index >= 75) {
      std::cerr << "Error: Trying to set local greater than allowed amount! (75)" << std::endl;
      exit(1);
    }

    locals[index] = val;
  }

  AdkValue VM::getLocal(int index) {
    if (index >= 75) {
      std::cerr << "Error: Trying to get local greater than allowed amount! (75)" << std::endl;
      exit(1);
    }

    return locals[index];
  }

  void VM::setGlobal(int index, AdkValue val) {
    if ((size_t)index >= globals.size() || globals.size() <= 0) {
      size_t globalSize = globals.size();
      for (size_t i = globalSize; i <= (size_t)index; i++) {
        globals.push_back(AdkValue());
      }
    }

    globals[index] = val;
  }

  AdkValue VM::getGlobal(int index) {
    if ((size_t)index >= globals.size()) {
      std::cerr << "Error: Trying to fetch global that does not exist!" << std::endl;
      exit(1);
    }

    return globals[index];
  }

  void VM::push(AdkValue val) {
    stack.size++;

    if (stack.size >= 512) {
      std::cerr << "Error: Stack Overflow!" << std::endl;
      exit(1);
    }

    // std::cout << "PUSH: " << val.as.number << std::endl;

    *sp = val;
    sp++;
  }
  AdkValue VM::pop() {
    sp--;
    stack.size--;

    if (stack.size < 0) {
      std::cerr << "Error: Stack Underflow!" << std::endl;
      exit(1);
    }

    AdkValue value = *sp;

    // std::cout << "POP: " << value.as.number << std::endl;

    return value;
  }

  AdkValue VM::run() {
    createLocals();
    globals.reserve(10);

    for (;;) {
      u8 instruction = READ_BYTE();
      // printStack();

      switch (instruction) {
        case HALT:
          return stack.size > 0 ? pop() : AdkValue(0);

        case LD_CONST: {
          AdkValue constant = constants[READ_BYTE()];
          push(constant);
          break;
        }

        case SETGLBL: {
          AdkValue index = AdkValue((int)READ_BYTE());
          AdkValue value = pop();

          setGlobal((int)index.as.number, value);

          break;
        }

        case GETGLBL: {
          AdkValue index = AdkValue((int)READ_BYTE());

          push(getGlobal((int)index.as.number));
          break;
        }
        
        case GETLCL: {
          AdkValue index = AdkValue((int)READ_BYTE());
          AdkValue localValue = getLocal((int)index.as.number);

          push(localValue);
          break;
        }

        case SETLCL: {
          AdkValue index = AdkValue((int)READ_BYTE());
          AdkValue value = pop();

          setLocal((int)index.as.number, value);
          break;
        }

        case JMP: {
          u8 addrhigh = READ_BYTE();
          u8 addrlow = READ_BYTE();

          uint16_t addr = ((uint16_t)addrhigh << 8) | addrlow;
          
          ip = &code[addr];
          break;
        }

        case JMP_NT: {
          u8 addrhigh = READ_BYTE();
          u8 addrlow = READ_BYTE();

          uint16_t addr = ((uint16_t)addrhigh << 8) | addrlow;

          AdkValue val = pop();

          if (val.as.number >= 1) {
            break;
          }

          ip = &code[addr];
          break;
        }

        case JMP_T: {
          u8 addrhigh = READ_BYTE();
          u8 addrlow = READ_BYTE();

          uint16_t addr = ((uint16_t)addrhigh << 8) | addrlow;

          AdkValue val = pop();

          if (val.as.number >= 1) {
            ip = &code[addr];
            break;
          }

          break;
        }

        case CMP_EQ: {
          AdkValue left = pop();
          AdkValue right = pop();

          AdkValue boolean;

          if (left.type == DataTypes::Int || left.type == DataTypes::Double) {
            boolean = AdkValue(left.as.number == right.as.number);
          }

          push(boolean);
          break;
        }

        case CMP_GT: {
          AdkValue left = pop();
          AdkValue right = pop();

          AdkValue boolean;

          if (left.type == DataTypes::Int || left.type == DataTypes::Double) {
            boolean = AdkValue(left.as.number > right.as.number);
          }

          push(boolean);
          break;
        }

        case CMP_GTE: {
          AdkValue left = pop();
          AdkValue right = pop();

          AdkValue boolean;

          if (left.type == DataTypes::Int || left.type == DataTypes::Double) {
            boolean = AdkValue(left.as.number >= right.as.number);
          }

          push(boolean);
          break;
        }

        case CMP_LT: {
          AdkValue left = pop();
          AdkValue right = pop();

          AdkValue boolean = AdkValue(false);

          if (left.type == DataTypes::Int || left.type == DataTypes::Double) {
            boolean = AdkValue(left.as.number < right.as.number);
          }

          push(boolean);
          break;
        }

        case CMP_LTE: {
          AdkValue left = pop();
          AdkValue right = pop();

          AdkValue boolean = AdkValue(false);

          if (left.type == DataTypes::Int || left.type == DataTypes::Double) {
            boolean = AdkValue(left.as.number <= right.as.number);
          }

          push(boolean);
          break;
        }

        case STRELCLS: {
          int argsMax = (int)READ_BYTE();
          AdkValue thisObj = AdkValue();
          AdkValue argCount = pop();

          bool isMethod = false;

          if (argCount.type == DataTypes::Object) {
            thisObj = argCount;
            argCount = pop();

            setLocal(0, thisObj);
            isMethod = true;
          }

          for (int i = 0; i < (int)argCount.as.number; i++) {
            if (i >= argsMax) {
              break;
            }
            AdkValue arg = pop();
            setLocal(isMethod ? i + 1 : i, arg);
          }

          break;
        }

        case CALL: {
          u8 addrhigh = READ_BYTE();
          u8 addrlow = READ_BYTE();

          uint16_t addr = ((uint16_t)addrhigh << 8) | addrlow;

          pushLocals();
          rp.push_back(ip);

          ip = &code[addr];
          createLocals();
          
          break;
        }

        case NEW: {
          u8 byte = READ_BYTE();
          AdkValue constant = constants[byte];

          if (constant.type != DataTypes::Object) {
            std::cerr << "Error: Constant not an object! Can only copy objects!" << std::endl;
            exit(1);
          }
          AdkValue objPtr = copyObject(constant.as.obj);

          push(objPtr);
          break;
        }

        case GETFIELD: {
          AdkValue prop = pop();
          AdkValue objPtr = pop();

          if (objPtr.type != DataTypes::Object) {
            std::cerr << "Error: Cannot get field of non object!" << std::endl;
            exit(1);
          }

          string propName = (
            prop.type == DataTypes::Object
            && prop.as.obj->objType == "String"
          ) ? ((AdkString*)prop.as.obj)->toString()
            : std::to_string((int)prop.as.number);

          if (!objPtr.as.obj->hasProp(propName)) {
            std::cerr << "Error: Property " << propName << " does not exist!" << std::endl;
            exit(1);
          }

          AdkValue field = objPtr.as.obj->props[propName];
          push(field);
          break;
        }

        case SETFIELD: {
          AdkValue item = pop();
          AdkValue prop = pop();
          AdkValue objPtr = pop();

          if (objPtr.type != DataTypes::Object) {
            std::cerr << "Error: Cannot get field of non object!" << std::endl;
            exit(1);
          }

          string propName = (
            prop.type == DataTypes::Object
            && prop.as.obj->objType == "String"
          ) ? ((AdkString*)prop.as.obj)->toString()
            : std::to_string((int)prop.as.number);

          objPtr.as.obj->props[propName] = item;
          push(objPtr);
          break;
        }

        case DUPE: {
          AdkValue value = pop();
          push(value);
          push(value);
          break;
        }

        case SWAP: {
          AdkValue valueTop = pop();
          AdkValue valueBottom = pop();

          push(valueTop);
          push(valueBottom);
          break;
        }

        case INVOKEMETHOD: {
          AdkValue method = pop();
          AdkValue objPtr = pop();
          // pop() -> Argcount
          // pop(n) -> Args
          AdkObject* obj = objPtr.as.obj;

          if (objPtr.type != DataTypes::Object) {
            AdkError err = AdkError("Can only call methods off of objects!");
            err.throwErr();
          }

          string methodName = (
            method.type == DataTypes::Object
            && method.as.obj->objType == "String"
          ) ? ((AdkString*)method.as.obj)->toString()
            : std::to_string((int)method.as.number);
          
          if (!obj->hasMethod(methodName)) {
            std::cerr << "Error: Method " << methodName << " does not exist!" << std::endl;
            exit(1);
          }

          Method methodPtr = obj->methods[methodName];

          if (methodPtr.func != nullptr) {
            AdkValue argCount = pop();
            AdkValue* args = new AdkValue[(int)argCount.as.number];

            for (int i = 0; i < (int)argCount.as.number; i++) {
              args[i] = pop();
            }
            AdkValue val = methodPtr.func((int)argCount.as.number, args, obj);
            push(val);
            break;
          }

          u16 addr = methodPtr.addr;
          push(objPtr);

          pushLocals();
          rp.push_back(ip);

          ip = &code[addr];
          createLocals();
          
          break;
        }

        case IMPORT: {
          // AdkValue globalNum = AdkValue((int)READ_BYTE());
          AdkValue file = pop();

          // int globalNumInt = (int)globalNum.as.number;

          if (file.type != DataTypes::Object) {
            AdkError err = AdkError("Import path must be a string!");
            err.throwErr();
          }

          string fileAsStr = ((AdkString*)file.as.obj)->toString();
          bool isBuiltIn = hasModule(fileAsStr);

          if (isBuiltIn && modules[fileAsStr].entry != nullptr) {
            AdkModule adkmodule = modules[fileAsStr];
            NativeModule moduleEntry = adkmodule.entry;

            AdkValue exportedVal = moduleEntry(this);
            push(exportedVal);

            // if (globalNumInt == -1) {
            //   AdkError err = AdkError("Native modules must be imported into a variable!");
            //   err.throwErr();
            // } else if (globalNumInt >= 0) {
            //   setLocal(globalNumInt, exportedVal);
            // } else {
            //   AdkError err = AdkError("Import must contain a specified global to store!");
            //   err.throwErr();
            // }

            break;
          }

          string filePath = !isBuiltIn
            ? fileAsStr
            : modules[fileAsStr].modulePath;
          
          Program prog;

          VM vm = VM();
          vm.syscalls = syscalls;
          vm.modules = modules;

          vm.readProgramFile(filePath, &prog);
          AdkValue exportedVal = vm.run();

          push(exportedVal);

          // if (globalNumInt == -1) {
          //   vm.run();
          //   VMState newState = vm.saveState();

          //   appendState(newState);
          // } else if (globalNumInt >= 0) {
          //   AdkValue exportedVal = vm.run();

          //   setLocal(globalNumInt, exportedVal);
          // } else {
          //   AdkError err = AdkError("Import must contain a specified global to store!");
          //   err.throwErr();
          // }

          break;
        }

        case RETURN: {
          // AdkValue returnVal = pop();
          
          popLocals();

          ip = rp[rp.size() - 1];
          rp.pop_back();

          // push(returnVal);

          break;
        }

        case RETURNVOID: {
          popLocals();

          ip = rp[rp.size() - 1];
          rp.pop_back();

          break;
        }

        case PUSH: {
          AdkValue value = AdkValue((int)READ_BYTE());
          push(value);

          break;
        }

        case POP: {
          pop();
          break;
        }

        case CONST_ADD: {
          AdkValue const1 = pop();
          AdkValue const0 = pop();

          if (const1.isString() && const0.isString()) {
            push(const1.toString() + const0.toString());
            break;
          }

          push(AdkValue(const1.as.number + const0.as.number));
          break;
        }

        case CONST_SUB: {
          AdkValue const1 = pop();
          AdkValue const0 = pop();

          push(AdkValue(const1.as.number - const0.as.number));
          break;
        }

        case CONST_MUL: {
          AdkValue const1 = pop();
          AdkValue const0 = pop();

          push(AdkValue(const1.as.number * const0.as.number));
          break;
        }

        case CONST_DIV: {
          AdkValue const1 = pop();
          AdkValue const0 = pop();

          push(AdkValue(const1.as.number / const0.as.number));
          break;
        }

        case CONST_MDO: {
          AdkValue const1 = pop();
          AdkValue const0 = pop();

          push(AdkValue(((int)const1.as.number) % ((int)const0.as.number)));
          break;
        }

        case SYSCALL: {
          AdkValue syscallVal = pop();
          AdkValue argCount = pop();
          AdkValue* args = new AdkValue[(int)argCount.as.number];

          if (syscalls.capacity() < (size_t)syscallVal.as.number) {
            fprintf(stderr, "Invalid system call!\n");
            LOGINSTR(instruction);
            throw std::exception();
          }

          for (int i = 0; i < (int)argCount.as.number; i++) {
            args[i] = pop();
          }

          // LOG(syscallVal);
          // LOG(argCount);

          AdkValue returnVal = syscalls[(int)syscallVal.as.number]((int)argCount.as.number, args, nullptr);

          if (returnVal.type != DataTypes::None) {
            push(returnVal);
          }
          break;
        }


        default:
          fprintf(stderr, "Invalid instruction\n");
          LOGINSTR(instruction);
          throw std::exception();
      }
    }
  }

  AdkValue VM::run(u8* code) {
    this->code = code;
    ip = &code[0];

    return run();
  };

  void VM::define(size_t sysNum, NativeFunction func) {
    if (sysNum >= syscalls.size()) {
      syscalls.resize(sysNum + 1);
    }

    syscalls[sysNum] = func;
  }

  void VM::writeProgramToFile(std::string filename, Program prog) {
    uint8_t magic = 0b11011101;
    uint8_t version = VERSION; // Version 0.1
    uint8_t poolsize = prog.constants.size();
    std::vector<Constant> constants = prog.constants;
    std::vector<u8> instructions = prog.instructions;

    std::ofstream file(filename, std::ios::binary);

    file.write((const char*)&magic, sizeof(magic));
    file.write((const char*)&version, sizeof(version));
    file.write((const char*)&poolsize, sizeof(poolsize));

    // Write constants to file
    for (int i = 0; i < (int)poolsize; i++) {
      uint16_t constsize = constants[i].size;
      uint8_t* constant = constants[i].constant;

      file.write((const char*)&constsize, sizeof(constsize));

      for (int j = 0; j < (int)constsize; j++) {
        file.write((const char*)&constant[j], sizeof(constant[j]));
      }
    }

    // Write instructions to file
    for (auto instruction : instructions) {
      file.write((const char*)&instruction, sizeof(instruction));
    }

    file.close();
  }


  void VM::readProgramFile(std::string filename, Program* prog) {
    std::ifstream file(filename);

    if (!file.good()) {
      file.close();
      // std::cerr << "Error: Could not open file: " << filename << std::endl;
      AdkError err = AdkError("Could not open file: ");
      err.append(filename);
      err.throwErr();
      return;
    }

    uint8_t magic;
    uint8_t version; // Version 0.1
    uint8_t poolsize;

    file.read((char*)&magic, sizeof(magic));
    file.read((char*)&version, sizeof(version));

    if (magic != MAGICNUMBER) {
      file.close();
      // std::cerr << "Error: Incorrect file type, malformed file, or incorrect magic number!" << std::endl;
      // exit(1);
      AdkError err = AdkError("Incorrect file type, malformed file, or incorrect magic number!");
      err.throwErr();
    }

    if (version > VERSION) {
      file.close();
      AdkError err = AdkError("Cannot read file as it is a higher version! Please update VM!");
      err.throwErr();
    } else if (version < VERSION) {
      // std::cout << "Warning:\x1b[0m Version mismatch might cause issues! Please update program!" << std::endl;
      AdkWarning warning = AdkWarning("Version mismatch might cause issues! Please update program!");
      warning.printWarning();
      std::cout << "Running Anyway..." << std::endl;
    }

    file.read((char*)&poolsize, sizeof(poolsize));

    constants = new AdkValue[(int)poolsize];
    constantSize = (size_t)poolsize;
    
    for (int i = 0; i < (int)poolsize; i++) {
      uint8_t type;
      uint16_t constsize;

      file.read((char*)&type, sizeof(type));
      file.read((char*)&constsize, sizeof(constsize));
      uint8_t* constant = new uint8_t[constsize];
      
      for (int j = 0; j < (int)constsize; j++) {
        uint8_t byte;
        file.read((char*)&byte, sizeof(byte));

        constant[j] = byte;
      }

      Constant conststruct{
        type,
        constsize,
        constant
      };

      prog->constants.push_back(conststruct);

      if (type == 0) {
        constants[i] = createObject(std::string((const char*)constant, (size_t)constsize), false); // Creates string
      } else if (type == 1) {
        constants[i] = createObject("Object", true);
        
        for (int j = 0; j < (int)constsize; j += 3) {
          uint8_t methodNameSize = constant[j + 2];
          string name = "";

          for (int k = 0; k < (int)methodNameSize; k++) {
            name += constant[j + 3 + k];
          }
          
          if (name[0] != '~') {
            name = name.substr(0, name.find("("));
          }

          constants[i].as.obj->methods[name] = Method{
            (u16)(((u16)constant[j] << 8) | constant[j + 1]),
            name,
            nullptr
          };

          j += (int)methodNameSize;
        }
      }
    }

    while (true) {
      u8 instruction;
      file.read((char*)&instruction, sizeof(instruction));
      if (file.eof()) break;
      if (!file.good()) {
        file.close();
        std::cerr << "Error: Could not read file! -> Could not read instructions!" << std::endl;
        exit(1);
      }

      prog->instructions.push_back(instruction);
    }

    code = prog->instructions.data();
    this->ip = &code[0];

    file.close();
  }

}; // namespace Aardvark
