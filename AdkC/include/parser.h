#pragma once
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

namespace Aardvark {
  using std::vector;
  using std::string;

  class Parser {
    public:
    Parser();
    Parser(vector<Token> tokens);

    Token advance(int amt);
    Token peek(int amt);
    
    bool isEOF();
    bool isIgnore(Token tok);

    void skipIgnore();
    void skipLinebreak();
    void skipOver(string val, Token tok);
    void skipOver(TokenTypes type, Token tok);
    void skipOver(TokenTypes type, string val, Token tok);

    vector<AST*> pDelimiters(string start, string end, string separator);

    bool isBlockMerger(Token tok);
    
    bool isCallable(AST* callStmt);
    AST* checkCall(AST* expr);
    AST* pCall(AST* expr);
    
    AST* pBinary(AST* left, int prec);

    AST* pAccess(AST* expr);

    AST* pAttribute(AST* expr);
    AST* pIdentifier(AST* expr);

    // AST* pSet();
    AST* pFunction(bool isStatic);
    AST* pReturn();

    AST* pClass();
    AST* pImport();

    AST* pIf();
    AST* pFor();
    AST* pWhile();
    AST* pContinue();
    AST* pBreak();

    AST* pArray();

    AST* pInclude();
    AST* pDirective();

    AST* pAll();
    AST* $pAll();

    AST* pExpression();
    AST* parse();

    private:
    vector<Token> tokens;
    Token curTok;

    int index = 0;
  };

}; // namespace Aardvark

#endif