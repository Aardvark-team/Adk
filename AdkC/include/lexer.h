#pragma once
#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "token.h"

namespace Aardvark {
  using std::vector;
  using std::string;

  class Lexer {
    public:
    Lexer();
    Lexer(string input);

    char advance();
    char advance(int amt);

    char peek();
    char peek(int amt);

    string grab();
    string grab(int amt);

    bool isSpecial(char c);
    bool isWhitespace(char c);
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isNumber(char c);
    bool isQuote(char c);
    bool isQuote(char c, char quote);
    int  isOperator(char c);
    bool isDelimiter(char c);

    vector<Token> tokenize();
    vector<Token> tokenize(string input);

    void reset();

    private:
    string input;
    vector<Token> tokens;

    int index;
    int line;
    int column;

    char curChar;
  };

}; // namespace Aardvark

#endif