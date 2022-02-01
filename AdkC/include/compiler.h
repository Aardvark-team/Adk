#pragma once
#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
#include "filereader.h"

namespace Aardvark {
  using std::map;

  class Variable;

  class Constant {
    public:
    int index;
    string type;
    vector<string> values;

    Variable* thisObj = nullptr;
    bool isClass = false;

    Constant();
    Constant(int index, string type);
    Constant(int index, string type, string value);
  };

  class ConstantList {
    public:
    map<string, Constant> constants;
    int constantsSize = 0;


    void addConstant(string name, Constant constant);
    bool hasConstant(string name);

    Constant getConstant(string name);
    Constant getConstantByIndex(int index);
  };

  class Variable {
    public:
    int index;
    string name;
    string type;
    string objType = "None"; // only if its an obj

    map<string, Variable> props;
    int propSize = 0; // for objects only
    int methodSize = 0; // for objects only

    string value;

    Variable();
    Variable(int index, string name, string type, string value);
    Variable(int index, string name, string type, string objType, string value);

    void SetObjType(string type);

    bool hasProp(string name);
    Variable getProp(string name);

    void addProp(string name, Variable variable);
    void addMethod(string name, Variable method);
  };

  class Function {
    public:
    string name;
    vector<string> args;
    vector<AST*> astArgs;
    vector<AST*> body;

    map<string, string> labels;

    bool hasReturn = false;
    bool isMethod = false;

    map<string, Variable> variables;
    int variablesSize = 0;

    Function(string name, vector<string> args);
    Function(AST* exp);
    Function();

    bool hasVariable(string name);
    Variable getVariable(string name);

    void addVariable(string name, Variable& variable);

    bool hasLabel(string name);
    string getLabel(string name);
    string createLabel(string code);

    string getFunctionLabel() const;
  };

  class Compiler {
    public:
    AST* ast;
    ConstantList constants;
    map<string, Function> functions;

    Function currentFunction;
    string currentFile = "";

    Compiler(AST* ast);

    string createClass(AST* exp);
    string createFunction(AST* exp);
    string createFunctionCall(AST* exp);
    string createReturn(AST* exp);
    string createIdentifier(AST* exp);
    string createPropAccess(AST* exp, Variable parentVar);
    string createAssign(AST* exp);
    string createNewObject(AST* exp, Constant classConstant); // aka 'new' keyword
    string createBinary(AST* exp);
    string createWhile(AST* exp);
    string createIf(AST* exp);

    string createInclude(AST* exp);

    Constant createString(string str);
    string createString(AST* exp);
    string createNumber(AST* exp);


    string codeGen(AST* exp);

    string compile();

    static string compileFile(string filepath);
  };

}; // namespace Aardvark

#endif