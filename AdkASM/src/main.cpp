#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <unordered_map>
#include "../include/program.h"
#include "../include/opcodes.h"

// this whole file is a monstrosity just gunna let you know

#define LOGINSTR(value) (std::cout << #value << " = 0x" << std::setfill('0') << std::setw(2) << std::hex << (int)value << "\n")

using namespace Aardvark;

// Got from stack overflow too lazy to implement myself
// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

std::vector<std::string> split(const std::string& str, const std::string delimiter, int amt = 0) {
  std::vector<std::string> lines = {};

  size_t pos = 0;
  size_t oldpos = 0;
  std::string current;

  int iter = 0;

  while((pos = str.find(delimiter, oldpos)) != std::string::npos) {
    if (iter >= amt && amt != 0) {
      break;
    }
    current = str.substr(oldpos, pos - oldpos);

    if (current.size() > 0) {
      lines.push_back(current);
    }
    
    oldpos = pos + delimiter.length();
    iter++;
  }

  if (oldpos < str.length()) {
    lines.push_back(str.substr(oldpos));
  }

  return lines;
}

void writeProgramToFile(std::string filename, Program prog) {
  uint8_t magic = MAGICNUMBER;
  uint8_t version = VERSION; // Version 0.1
  uint8_t poolsize = prog.constants.size();
  std::vector<Constant> constants = prog.constants;
  std::vector<uint8_t> instructions = prog.instructions;

  std::ofstream file(filename, std::ios::binary);

  file.write((const char*)&magic, sizeof(magic));
  file.write((const char*)&version, sizeof(version));
  file.write((const char*)&poolsize, sizeof(poolsize));

  // Write constants to file
  for (int i = 0; i < (int)poolsize; i++) {
    uint8_t type = constants[i].type;
    uint16_t constsize = constants[i].size;
    uint8_t* constant = constants[i].constant;

    file.write((const char*)&type, sizeof(type));
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

std::string readAdkASM(std::string filename) {
  std::ifstream file(filename);

  std::string line;
  std::string data = "";

  bool isFirst = true;

  while(std::getline(file, line)) {
    if (isFirst) {
      isFirst = false;
    } else {
      data += "\n";
    }

    if (line.find_first_not_of(" ") != std::string::npos) {
      size_t commentPos = 0;
      if ((commentPos = line.find(";")) != std::string::npos) {
        line.erase(commentPos, line.length());
      }
      trim(line);
      data += line;
    }
  }

  return data;
}

std::unordered_map<uint8_t, std::string> U82Instruction = {
  {HALT, "halt"},
  {CONST_ADD, "const_add"},
  {CONST_SUB, "const_sub"},
  {CONST_MUL, "const_mul"},
  {CONST_DIV, "const_div"},
  {CONST_MDO, "const_mod"},

  {LD_CONST, "load_const"},

  {PUSH, "push"},
  {POP, "pop"},

  {SYSCALL, "syscall"},

  {SETGLBL, "setglobal"},
  {GETGLBL, "getglobal"},
  {SETLCL, "setlocal"},
  {GETLCL, "getlocal"},

  {STRELCLS, "storelcls"},

  {CALL, "call"},
  {INVOKEMETHOD, "invokemethod"},
  {RETURN, "return"},
  {RETURNVOID, "returnvoid"},

  {CMP_EQ, "cmp_eq"},
  {CMP_LT, "cmp_lt"},
  {CMP_LTE, "cmp_lte"},
  {CMP_GT, "cmp_gt"},
  {CMP_GTE, "cmp_gte"},

  {JMP, "jmp"},
  {JMP_T, "jmp_t"},
  {JMP_NT, "jmp_nt"},

  {NEW, "new"},
  {SETFIELD, "setfield"},
  {GETFIELD, "getfield"},
  {DUPE, "dupe"},
  {SWAP, "swap"},

  {IMPORT, "import"}
};

std::unordered_map<std::string, uint8_t> instruction2U8 = {
  {"halt", HALT},
  {"const_add", CONST_ADD},
  {"const_sub", CONST_SUB},
  {"const_mul", CONST_MUL},
  {"const_div", CONST_DIV},
  {"const_mod", CONST_MDO},

  {"load_const", LD_CONST},

  {"push", PUSH},
  {"pop", POP},

  {"syscall", SYSCALL},

  {"setglobal", SETGLBL},
  {"getglobal", GETGLBL},
  {"setlocal", SETLCL},
  {"getlocal", GETLCL},

  {"storelcls", STRELCLS},

  {"call", CALL},
  {"invokemethod", INVOKEMETHOD},
  {"return", RETURN},
  {"returnvoid", RETURNVOID},

  {"cmp_eq", CMP_EQ},
  {"cmp_lt", CMP_LT},
  {"cmp_lte", CMP_LTE},
  {"cmp_gt", CMP_GT},
  {"cmp_gte", CMP_GTE},

  {"jmp", JMP},
  {"jmp_t", JMP_T},
  {"jmp_nt", JMP_NT},

  {"new", NEW},
  {"setfield", SETFIELD},
  {"getfield", GETFIELD},
  {"dupe", DUPE},
  {"swap", SWAP},


  {"import", IMPORT}
};

uint8_t instructionToU8(std::string instr) {
  if (instruction2U8.find(instr) == instruction2U8.end()) {
    std::cerr << "Error: Invalid instruction, '" << instr << "'\n";
    exit(1);
  }

  return instruction2U8[instr];
}

std::string U8ToInstruction(uint8_t instr) {
  if (U82Instruction.find(instr) == U82Instruction.end()) {
    return "";
  }

  return U82Instruction[instr];
}

Program assembleProgram(std::vector<std::string> lines) {
  Program prog;

  std::vector<Constant> constants = {};
  std::vector<uint8_t> instructions = {};
  size_t totalLength = 0;
  
  std::unordered_map<std::string, size_t> labels;

  std::string constEnd = "";

  size_t constLength = 0;
  bool isConstants = true;
  
  for (size_t i = 0; i < lines.size(); i++) {
    std::string line = lines[i];
    if (isConstants) {
      if (line == "&consts_end:") {
        isConstants = false;
      }
      continue;
    }

    std::vector<std::string> args = split(line, " ");
    totalLength += args.size();

    if (
      args[0] == "jmp" ||
      args[0] == "jmp_nt" ||
      args[0] == "jmp_t" ||
      args[0] == "call"
    ) {
      totalLength += 1;
    }

    size_t colonPos = 0;
    
    if ((colonPos = line.find(":")) == std::string::npos) {
      continue;
    }
    totalLength -= args.size();

    std::string labelName = line.substr(0, colonPos);
    labels[labelName] = totalLength;
  }

  while (constEnd != "&consts_end:") {
    constEnd = lines[constLength];
    std::string& line = constEnd;

    if (line == "&consts:") {
      constLength++;
      constEnd = lines[constLength];
      continue;
    }

    std::vector<std::string> info = split(line, ",", 2);
    if (info.size() <= 1) {
      continue;
    }

    size_t index = (size_t)std::stoi(info[0]);
    std::string type = info[1];
    std::string data = info[2];
    trim(data);
    trim(type);

    if (index >= constants.size() || constants.size() <= 0) {
      size_t constantSize = constants.size();
      for (size_t ind = constantSize; ind <= index; ind++) {
        constants.push_back({});
      }
    }

    bool isString = type == "String" && (data[0] == '\"' || data[0] == '\'');
    if (!isString && type != "Object") {
      std::cerr << "Error: Only strings and objects are currently supported as constants!" << std::endl;
      exit(1);
    }

    size_t size = isString ? data.length() - 2 : data.length();

    if (type == "Object") {
      std::string objLine = data;
      std::vector<uint8_t> methods;

      while (objLine != "}") {
        if ((size_t)constLength >= lines.size()) {
          std::cerr << "Error: Malformed object!" << std::endl;
          exit(1);
        }
        
        std::string method = "";
        for (size_t i = 0; i < objLine.size(); i++) {
          char c = objLine[i];
          if (c == '{') {
            continue;
          }

          if (c == ',') {
            trim(method);
            size_t label = labels[method];
            methods.push_back(label >> 8);
            methods.push_back(label & 0xFF);
            methods.push_back((uint8_t)method.size());
            for (char c : method) {
              methods.push_back((uint8_t)c);
            }
            method = "";
            continue;
          }

          if (c != '}') {
            method += c;
          } else {
            objLine = "}";
          }
        }

        if (method.length() > 0) {
          trim(method);
          size_t label = labels[method];
          methods.push_back(label >> 8);
          methods.push_back(label & 0xFF);
          methods.push_back((uint8_t)method.size());
          for (char c : method) {
            methods.push_back((uint8_t)c);
          }
          method = "";
        }

        constLength++;
        constEnd = lines[constLength];

        if (objLine == "}") {
          break;
        }

        objLine = lines[constLength];
      }

      size = methods.size();
      isString = false;
      
      uint8_t* str = new uint8_t[size];

      for (size_t i = 0; i < size; i++) {
        str[i] = methods[i];
      }

      constants[index] = Constant{
        (uint8_t)1,
        (uint16_t)size,
        str
      };

      if (constEnd == "}") {
        constLength++;
        constEnd = lines[constLength];
      }
      continue;
    }

    uint8_t* str = new uint8_t[size];

    for (size_t i = 0; i < size + 1; i++) {
      str[i] = data[isString ? i + 1 : i];
    }
    constants[index] = Constant{
      (uint8_t)0,
      (uint16_t)size,
      str
    };

    constLength++;
    constEnd = lines[constLength];
  }

  constLength++;

  size_t currentLabel = 0;

  for (size_t i = constLength; i < lines.size(); i++) {
    std::string line = lines[i];
    std::vector<std::string> args = split(line, " ");

    size_t colonPos = 0;
    if ((colonPos = line.find(":")) != std::string::npos) {
      currentLabel = labels[line.substr(0, colonPos)];
      continue;
    }

    std::string instrString = args[0];
    uint8_t instr = instructionToU8(instrString);

    if (instrString == "call") {
      std::string arg = args[1];

      int label = (int)labels[arg];
      instructions.push_back(instr);
      instructions.push_back(label >> 8);
      instructions.push_back(label & 0xFF);
      continue;
    }

    if (instrString == "jmp" || instrString == "jmp_nt" || instrString == "jmp_t") {
      std::string arg = args[1];
      
      int label = (int)labels[arg];
      instructions.push_back(instr);
      instructions.push_back(label >> 8);
      instructions.push_back(label & 0xFF);
      continue;
    }
    
    instructions.push_back(instr);

    if (args.size() > 1) {
      std::string arg = args[1];

      int argAsInt = std::stoi(arg);
      instructions.push_back((uint8_t)argAsInt);
    }
  }

  prog.instructions = instructions;
  prog.constants = constants;

  return prog;
}

int main(int argc, char** argv) {
  // Create a argv handler; Please so its better than the current
  if (argc < 3) {
    std::cout << "Usage: adkasm <input> <output>\n";
    return 1;
  }
  std::string adkasm = readAdkASM(argv[1]);
  std::vector<std::string> lines = split(adkasm, "\n");

  Program prog = assembleProgram(lines);
  writeProgramToFile(argv[2], prog);

  return 0;
}