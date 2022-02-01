#pragma once
#ifndef TOKEN_H
#define TOKEN_H
#include "common.h"

namespace Aardvark {
  using std::string;

  enum class TokenTypes {
    None,
    String, // 1
    Int, // 2
    Double, // 3
    Boolean, // 4
    Keyword, // 5
    Identifier, // 6
    Operator, // 7
    Delimiter, // 8
    Linebreak, // 9
    Directive, // 10
    EndOfFile,
  };

  struct Position {
    int col = 0;
    int line = 0;
  };

  class Token {
    public:
    TokenTypes type;
    void* value;

    Position pos;

    Token();
    Token(TokenTypes type);
    Token(TokenTypes type, string value);
    Token(TokenTypes type, char value);
    Token(int value);
    Token(double value);
    Token(bool value);

    void updatePosition(Position pos);
    void updatePosition(int line, int col);

    bool isNull();

    template<typename T>
    T cast();
    string toString();
    
    bool equals(string value);
    bool equals(TokenTypes type, string value);

    bool equals(TokenTypes type);

    private:
  };

}; // namespace Aardvark

#endif