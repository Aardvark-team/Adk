#include <filesystem>
#include "../include/compiler.h"
#define MAINLABEL "__internal_main__"
#define NULLVAR "__NULL_VAR__"

namespace fs = std::filesystem;

namespace Aardvark {


  // Variable
  Variable::Variable() {
    this->index = -1;
    this->name = NULLVAR;
    this->type = "NULL";
    this->value = "null";
  }

  Variable::Variable(int index, string name, string type, string value) {
    this->index = index;
    this->name = name;
    this->type = type;
    this->value = value;
  }

  Variable::Variable(int index, string name, string type, string objType, string value) {
    this->index = index;
    this->name = name;
    this->type = type;
    this->objType = objType;
    this->value = value;
  }

  void Variable::SetObjType(string type) {
    this->objType = type;
  }

  bool Variable::hasProp(string name) {
    return props.find(name) != props.end();
  }
  
  Variable Variable::getProp(string name) {
    if (!hasProp(name)) {
      return Variable();
    }

    return props[name];
  }

  void Variable::addProp(string name, Variable variable) {
    props[name] = variable;
    propSize++;
  }

  void Variable::addMethod(string name, Variable method) {
    props[name] = method;
    methodSize++;
  }


  // Function
  Function::Function() {
    this->args = {};
    this->name = NULLVAR;
  }

  Function::Function(string name, vector<string> args) {
    this->args = args;
    this->name = name;
  }

  Function::Function(AST* exp) {
    astArgs = exp->args;
    args = {};

    variablesSize = 0;
    for (AST* arg : exp->args) {
      string argName = arg->value.toString();
      args.push_back(argName);

      variables[argName] = Variable(variablesSize, argName, "Any", "0");
      variablesSize++;
    }

    name = exp->value.toString();
    body = exp->block;
  }

  string Function::getFunctionLabel() const {
    string label = name;
    label += "(";

    bool isFirst = true;
    for (string arg : args) {
      if (isFirst) {
        isFirst = false;
      } else {
        label += ",";
      }
      
      label += arg;
    }

    label += ")";

    return label;
  };

  bool Function::hasVariable(string name) {
    return variables.find(name) != variables.end();
  }

  void Function::addVariable(string name, Variable& variable) {
    variablesSize++;

    variables[name] = variable;
  }

  Variable Function::getVariable(string name) {
    if (!hasVariable(name)) {
      std::cerr << "Error: Cannot get variable '" << name << "' as it is not defined.\n";
      exit(1); // TODO: ERROR
    }

    return variables[name];
  }

  bool Function::hasLabel(string name) {
    return labels.find(name) != labels.end();
  }

  string Function::getLabel(string name) {
    if (!hasLabel(name)) {
      std::cerr << "Error: Cannot get label '" << name << "' as it is not defined.\n";
      exit(1); // TODO: ERROR
    }

    return labels[name];
  }
  
  string Function::createLabel(string code) {
    string name = getFunctionLabel();
    name += "_label";
    name += std::to_string(labels.size());

    labels[name] = code;

    return name;
  }


  // Compiler / ConstantList
  Compiler::Compiler(AST* ast): ast(ast) {};

  Constant::Constant() {
    this->index = -1;
    this->type = "NULL";
    this->values = {};
  }

  Constant::Constant(int index, string type) {
    this->index = index;
    this->type = type;
    this->values = {};
  }

  Constant::Constant(int index, string type, string value) {
    this->index = index;
    this->type = type;
    this->values = {value};
  }

  bool ConstantList::hasConstant(string name) {
    return constants.find(name) != constants.end();
  }

  void ConstantList::addConstant(string name, Constant constant) {
    constantsSize++;
    constants[name] = constant;
  }

  Constant ConstantList::getConstant(string name) {
    if (!hasConstant(name)) {
      std::cerr << "Error: Cannot get constant '" << name << "' as it is not defined.\n";
      exit(1); // TODO: 'ERROR'
    }

    return constants[name];
  }

  string Compiler::createClass(AST* exp) {
    string className = exp->value.toString();
    Constant constant = Constant(constants.constantsSize, "Object");
    constant.thisObj = new Variable(0, "this", "Object", "");
    constant.isClass = true;

    vector<AST*> block = exp->block;

    for (AST* expr : block) {
      if (expr->type != ASTTypes::Function) {
        continue;
      }

      Function func = Function(expr);
      func.isMethod = true;
      func.addVariable("this", *constant.thisObj);

      string name = func.name;
      name += "_METHOD";

      functions[name] = func;

      constant.values.push_back(func.getFunctionLabel());
    }

    constants.addConstant(className, constant);

    return "";
  }

  string Compiler::createFunction(AST* exp) {
    Function func = Function(exp);

    functions[func.name] = func;

    return "";
  }

  string Compiler::createFunctionCall(AST* exp) {
    string code = "";
    bool isChild = exp->parent != nullptr;
    bool isAccessing = exp->access != nullptr; // to be implemented
    bool isSubscript = exp->isSubscript;
    bool isGlobalScope = currentFunction.name == MAINLABEL;
    string funcName = exp->value.toString();

    bool isSyscall = syscalls.find(funcName) != syscalls.end();

    vector<AST*> args = exp->args;

    for (int i = args.size() - 1; i >= 0; i--) {
      AST* arg = args[i];
      string argCode = codeGen(arg);
      code += "\n";
      code += argCode;
    }
    code += "\n  push ";
    code += std::to_string(args.size());
    code += "\n  ";

    if (isChild && !isSubscript) {
      if (exp->parent->parent == nullptr) {
        code += (isGlobalScope)
          ? "getglobal "
          : "getlocal ";
        code += std::to_string(currentFunction.getVariable(exp->parent->value.toString()).index);
        code += "\n  ";
      }
      Constant constant = createString(funcName);
      code += "load_const ";
      code += std::to_string(constant.index);
      code += "\n  invokemethod";
      return code;
    }

    Constant classConstant;
    if (constants.hasConstant(funcName) && (classConstant = constants.getConstant(funcName)).isClass) {
      code += createNewObject(exp, classConstant);

      return code;
    } else if (!isSyscall && functions.find(funcName) == functions.end()) {
      std::cerr << "Error: Cannot call function '" << funcName << "' as it is not defined!\n";
      exit(1); 
    }

    Function func = functions[funcName];
    
    if (isSyscall) {
      code += "push ";
      code += std::to_string(syscalls[funcName]);
      code += "\n  syscall\n";
    } else {
      code += "call ";
      code += func.getFunctionLabel();
      code += "\n";
    }

    return code;
  }

  string Compiler::createReturn(AST* exp) {
    string code = "";

    if (exp->assign != nullptr) {
      code += codeGen(exp->assign);
    }

    code += "\n  return";
    currentFunction.hasReturn = true;

    return code;
  }

  string Compiler::createPropAccess(AST* exp, Variable parentVar) {
    string code = "\n  ";
    string varName = exp->value.toString();
    bool isAccessing = exp->access != nullptr;
    bool isChild = exp->parent != nullptr;
    bool isSubscript = exp->isSubscript;

    string accessCode = codeGen(exp);
    code += accessCode;

    if (exp->type != ASTTypes::FunctionCall) {
      code += "\n  getfield";
    }

    // Variable variable = parentVar.getProp(varName);

    if (isAccessing) {
      code += createPropAccess(exp->access, Variable()); // todo
    }

    return code;
  }

  string Compiler::createIdentifier(AST* exp) {
    // x = "Test"
    // output(x);
    // output(x.length)
    // output(x["length"]["size"])

    string code = "\n  ";
    string varName = exp->value.toString();
    bool isAccessing = exp->access != nullptr;
    bool isChild = exp->parent != nullptr;
    bool isSubscript = exp->isSubscript;
    bool isMethod = currentFunction.isMethod;
    bool isGlobalScope = currentFunction.name == MAINLABEL;

    Variable variable = isChild ? Variable() : currentFunction.getVariable(varName);

    if (isAccessing && variable.index == -1) {
      std::cerr
        << "Error: Cannot access property of '"
        << varName
        << "' as it is undefined!\n";
      exit(1); // TODO: ERROR
    }

    if (!isChild) {
      if (!isAccessing || exp->access->type != ASTTypes::FunctionCall) {
        code += (isGlobalScope)
          ? "getglobal "
          : "getlocal ";
        code += std::to_string(variable.index);
      }
    } else if (!isSubscript) {
      Constant constant = createString(varName);
      code += "load_const ";
      code += std::to_string(constant.index);
    } else {
      variable = currentFunction.getVariable(varName);
      if (!isAccessing || exp->access->type != ASTTypes::FunctionCall) {
        code += (isGlobalScope)
          ? "getglobal "
          : "getlocal ";
        code += std::to_string(variable.index);
      }
    }

    if (isAccessing) {
      code += createPropAccess(exp->access, variable); // todo
    }

    return code;
  }

  string Compiler::createAssign(AST* exp) {
    string name = exp->left->value.toString();
    bool isChild = exp->parent != nullptr;
    bool isSubscript = exp->isSubscript;
    bool isGlobalScope = currentFunction.name == MAINLABEL;
    bool isOpBefore = exp->isOpBefore;

    string op = exp->op.toString();

    string code = "";
    string fieldData = !isOpBefore
      ? codeGen(exp->right)
      : "";

    bool hasVariable = currentFunction.hasVariable(name);

    if (isChild) {
      Constant constant = createString(name);

      if (op != "=") {
        code += "\n  dupe";

        if (isOpBefore) {
          fieldData = "  swap\n  ";
        }

        if (op == "++") {
          fieldData += "push 1\n  const_add";
        } else if (op == "--") {
          fieldData += "push 1\n  swap\n  const_sub";
        } else if (op == "*=") {
          fieldData = string("  swap\n  ") + fieldData;
          fieldData += "\n  const_mul";
        } else if (op == "/=") {
          fieldData = string("  swap\n  ") + fieldData;
          fieldData += "\n  const_div";
        } else if (op == "+=") {
          fieldData = string("  swap\n  ") + fieldData;
          fieldData += "\n  const_add";
        } else if (op == "-=") {
          fieldData = string("  swap\n  ") + fieldData;
          fieldData += "\n  const_sub";
        } else if (op == "%=") {
          fieldData = string("  swap\n  ") + fieldData;
          fieldData += "\n  const_mod";
        }
      }

      code += "\n  load_const ";
      code += std::to_string(constant.index);
      code += "\n";
      code += fieldData;
      code += "\n  setfield";
      code += "\n  load_const ";
      code += std::to_string(constant.index);
    } else {
      Variable variable = hasVariable
        ? currentFunction.getVariable(name)
        : Variable(currentFunction.variablesSize, name, "Any", name);
      if (!hasVariable) {
        currentFunction.addVariable(name, variable);
      }

      if (op == "++") {
        fieldData = "\n  ";
        fieldData += isGlobalScope
          ? "getglobal "
          : "getlocal ";
        fieldData += std::to_string(variable.index);
        fieldData += "\n  push 1\n  const_add";
      } else if (op == "--") {
        fieldData = "\n  push 1\n  ";
        fieldData += isGlobalScope
          ? "getglobal "
          : "getlocal ";
        fieldData += std::to_string(variable.index);
        fieldData += "\n  const_sub";
      } else if (op == "+=") {
        string fieldDataTemp = fieldData;
        fieldData = "\n";
        fieldData += fieldDataTemp;
        fieldData += "\n  ";
        fieldData += isGlobalScope
          ? "getglobal "
          : "getlocal ";
        fieldData += std::to_string(variable.index);
        fieldData += "\n  const_add";
      } else if (op == "-=") {
        string fieldDataTemp = fieldData;
        fieldData = "\n  ";
        fieldData += isGlobalScope
          ? "getglobal "
          : "getlocal ";
        fieldData += std::to_string(variable.index);
        fieldData += "\n";
        fieldData += fieldDataTemp;
        fieldData += "\n  const_sub";
      } else if (op == "*=") {
        string fieldDataTemp = fieldData;
        fieldData = "\n  ";
        fieldData += isGlobalScope
          ? "getglobal "
          : "getlocal ";
        fieldData += std::to_string(variable.index);
        fieldData += "\n";
        fieldData += fieldDataTemp;
        fieldData += "\n  const_mul";
      } else if (op == "/=") {
        string fieldDataTemp = fieldData;
        fieldData = "\n  ";
        fieldData += isGlobalScope
          ? "getglobal "
          : "getlocal ";
        fieldData += std::to_string(variable.index);
        fieldData += "\n";
        fieldData += fieldDataTemp;
        fieldData += "\n  const_div";
      } else if (op == "%=") {
        string fieldDataTemp = fieldData;
        fieldData = "\n  ";
        fieldData += isGlobalScope
          ? "getglobal "
          : "getlocal ";
        fieldData += std::to_string(variable.index);
        fieldData += "\n";
        fieldData += fieldDataTemp;
        fieldData += "\n  const_mod";
      }

      if (!isOpBefore && (op == "++" || op == "--")) {
        code += isGlobalScope
          ? "\n  getglobal "
          : "\n  getlocal ";
        code += std::to_string(variable.index);
        code += "\n";
      }

      code += fieldData;
      code += isGlobalScope
        ? "\n  setglobal "
        : "\n  setlocal ";
      code += std::to_string(variable.index);

      if (isOpBefore) {
        code += isGlobalScope
          ? "\n  getglobal "
          : "\n  getlocal ";
        code += std::to_string(variable.index);
      }
    }

    return code;
  }

  string Compiler::createNewObject(AST* exp, Constant classConstant) {
    string code = "new ";
    code += std::to_string(classConstant.index);
    code += "\n  ";

    // TODO: check if ~init() exists

    // Constant initConst = Constant(constants.constantsSize, "String", "~init()");
    // constants.addConstant("~init()", initConst);
    Constant initConst = createString("~init()");

    code += "load_const ";
    code += std::to_string(initConst.index);
    code += "\n  invokemethod";

    return code;
  }

  string Compiler::createBinary(AST* exp) {
    string code = "\n";
    code += codeGen(exp->right);
    code += codeGen(exp->left);

    string op = exp->op.toString();

    if (op == "+") {
      code += "\n  const_add\n  ";
    } else if (op == "-") {
      code += "\n  const_sub\n  ";
    } else if (op == "*") {
      code += "\n  const_mul\n  ";
    } else if (op == "/") {
      code += "\n  const_div\n  ";
    } else if (op == "%") {
      code += "\n  const_mod\n  ";
    } else if (op == "<") {
      code += "\n  cmp_lt";
    } else if (op == "<=") {
      code += "\n  cmp_lte"; // TODO add new op
    } else if (op == ">") {
      code += "\n  cmp_gt";
    } else if (op == ">=") {
      code += "\n  cmp_gte"; // TODO add new op
    } else if (op == "==") {
      code += "\n  cmp_eq";
    }

    return code;
  }

  Constant Compiler::createString(string str) {
    if (constants.hasConstant(str)) {
      return constants.getConstant(str);
    }

    Constant constant = Constant(constants.constantsSize, "String", str);
    constants.addConstant(str, constant);

    return constant;
  }

  string Compiler::createString(AST* exp) {
    string constantValue = exp->value.toString();
    string constantName = constantValue;
    constantName += "__LIT";

    string code = "";

    if (constants.hasConstant(constantName)) {
      Constant constant = constants.getConstant(constantName);

      code += "  load_const ";
      code += std::to_string(constant.index);

      return code;
    }

    Constant newConstant = Constant(constants.constantsSize, "String", constantValue);
    constants.addConstant(constantName, newConstant);

    code += "  load_const ";
    code += std::to_string(newConstant.index);

    return code;
  }

  string Compiler::createNumber(AST* exp) {
    string numValue = exp->value.toString();
    if (numValue == "true") {
      numValue = "1";
    } else if (numValue == "false") {
      numValue = "0";
    }
    
    string code = "  push ";
    code += numValue;

    return code;
  }

  string Compiler::createWhile(AST* exp) {
    AST* condition = exp->condition;
    vector<AST*> block = exp->block;

    string label = currentFunction.createLabel("");
    string labelEnd = currentFunction.createLabel("");

    string code = "\n";
    code += label;
    code += ":\n  ";

    code += codeGen(condition);
    code += "\n  jmp_nt ";
    code += labelEnd;
    code += "\n  ";
    
    for (AST* expr : block) {
      code += codeGen(expr);
    }

    code += "\n  jmp ";
    code += label;
    code += "\n";
    code += labelEnd;
    code += ":\n  ";

    return code;
  }

  string Compiler::createIf(AST* exp) {
    AST* condition = exp->condition;
    vector<AST*> then = exp->block;

    AST* elseStmt = exp->els;
    vector<AST*> elseBlock = elseStmt != nullptr
      ? elseStmt->block
      : vector<AST*>();

    string label = currentFunction.createLabel("");
    string labelElse = currentFunction.createLabel("");
    string labelEnd = currentFunction.createLabel("");

    string code = "";
    code += codeGen(condition);
    code += "\n  jmp_nt ";
    code += labelElse;
    
    for (AST* expr : then) {
      code += "\n";
      code += codeGen(expr);
    }

    code += "\n";
    code += labelElse;
    code += ":\n";

    for (AST* expr : elseBlock) {
      code += "\n";
      code += codeGen(expr);
    }

    code += labelEnd;
    code += ":\n  ";

    return code;
  }

  string Compiler::createInclude(AST* exp) {
    string code = "\n  ";
    string importPath = exp->value.toString();
    bool hasIdentifier = exp->as != nullptr;
    bool isBuiltin = vincludes<string>(imports, importPath);

    fs::path fullfile = fs::path(currentFile).remove_filename() / fs::absolute(fs::path(importPath)).filename();
    std::string data = fullfile.string();

    if (!isBuiltin) {
      string path = "";
      size_t pos = data.find_last_of(".adk");
      bool hasFileExt = pos != string::npos;
      if (hasFileExt) {
        pos -= 3;
        path = data.substr(0, pos); // removes extension
      }
      
      string compiledCode = Compiler::compileFile(data);

      writeFile(path + ".adkb", compiledCode);

      importPath = path + ".adkc";
    }

    Constant importStr = createString(importPath);

    code += "load_const ";
    code += std::to_string(importStr.index);
    code += "\n  import ";

    if (hasIdentifier) {
      string identifierName = exp->as->value.toString();
      Variable variable = Variable(currentFunction.variablesSize, identifierName, "Object", identifierName, "");
      code += currentFunction.name == MAINLABEL
        ? "\n  setglobal "
        : "\n  setlocal ";
      code += std::to_string(variable.index);

      currentFunction.addVariable(identifierName, variable);
    } else if (isBuiltin) {
      string identifierName = exp->value.toString();
      Variable variable = Variable(currentFunction.variablesSize, identifierName, "Object", identifierName, "");
      code += currentFunction.name == MAINLABEL
        ? "\n  setglobal "
        : "\n  setlocal ";
      code += std::to_string(variable.index);

      currentFunction.addVariable(identifierName, variable);
    }

    return code;
  }

  string Compiler::codeGen(AST* exp) {
    switch (exp->type) {
      case ASTTypes::String:
        return createString(exp);
      case ASTTypes::Int:
      case ASTTypes::Double:
      case ASTTypes::Boolean:
        return createNumber(exp);

      case ASTTypes::Identifier:
        return createIdentifier(exp);

      case ASTTypes::Assign:
        return createAssign(exp);

      case ASTTypes::Function:
        return createFunction(exp);

      case ASTTypes::FunctionCall:
        return createFunctionCall(exp);

      case ASTTypes::Class:
        return createClass(exp);

      case ASTTypes::While:
        return createWhile(exp);

      case ASTTypes::If:
        return createIf(exp);

      case ASTTypes::Binary:
        return createBinary(exp);

      case ASTTypes::Return:
        return createReturn(exp);

      case ASTTypes::Include:
        return createInclude(exp);

      case ASTTypes::None:
        return "";
    }
  }

  string Compiler::compileFile(string filepath) {
    string data = readFile(filepath);
    
    Lexer lexer = Lexer(data);
    auto tokens = lexer.tokenize();

    Parser parser = Parser(tokens);
    AST* ast = parser.parse();

    Compiler compiler = Compiler(ast);
    compiler.currentFile = filepath;
    string code = compiler.compile();

    return code;
  }

  string Compiler::compile() {
    Function mainFunc = Function(MAINLABEL, {});
    string code = "";
    code += mainFunc.getFunctionLabel();
    code += ":\n";

    currentFunction = mainFunc;
    functions[MAINLABEL] = mainFunc;

    vector<AST*> block = ast->block;

    for (AST* exp : block) {
      code += codeGen(exp);
    }

    code += "\n  halt";

    for (auto const& [ key, val ] : functions) {
      if (val.name == MAINLABEL || val.name == NULLVAR) {
        continue;
      }
      Function oldFunc = currentFunction;
      vector<string> args = val.args;
      vector<AST*> body = val.body;
      code += "\n\n";
      code += val.getFunctionLabel();
      code += ":\n  ";
      code += "storelcls ";
      code += std::to_string(args.size());
      code += "\n";
      
      currentFunction = val;
      bool hasReturn = false;
      for (AST* exp : body) {
        code += "\n";
        code += codeGen(exp);
        hasReturn = currentFunction.hasReturn;
      }
      currentFunction = oldFunc;

      if (!hasReturn) {
        if (val.getFunctionLabel() == "~init()") {
          code += "\n  getlocal 0";
        }
        code += "\n  return";
      }
    }

    string constCode = "&consts:\n";

    for (auto const& [ key, val ] : constants.constants) {
      if (key == NULLVAR || val.type == "NULL") {
        continue;
      }
      constCode += "\n";
      constCode += std::to_string(val.index);
      constCode += ", ";
      constCode += val.type;

      if (val.type == "String") {
        constCode += ", \"";
        constCode += val.values[0];
        constCode += "\"";
      } else if (val.type == "Object") {
        constCode += ", { ";
        bool isFirst = true;
        for (string value : val.values) {
          if (isFirst) {
            isFirst = false;
          } else {
            constCode += ", ";
          }

          constCode += value;
        }
        constCode += " }";
      }
    }

    constCode += "\n\n&consts_end:\n\n";

    code = constCode + code;

    return code;
  }


}; // namespace Aardvark