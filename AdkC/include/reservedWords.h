#pragma once
#ifndef RESWORD_H
#define RESWORD_H
#include "token.h"
//Add bitwise operations later on

namespace Aardvark {
  using std::vector;
  using std::string;
  // Operators
  // used in lexer.cpp and lexer.h
  // Strings on the right sets the alias
  // This allows mutli definitions of a single operator

  // Each array index is the length of the operators
  // Index 1 means each operator in that array are of length 2
  // Used in parser.cpp, parser.h, lexer.cpp, and lexer.h
  inline vector<std::map<string, string>> operators = {
    {
      {"=", "="},
      {"+", "+"},
      {"-", "-"},
      {"*", "*"},
      {"/", "/"},
			{"^", "^"},
      {"%", "%"},
      {"<", "<"},
      {">", ">"},
			{"^", "^"},
			{"!", "!"} //not
    },
    {
      {"+=", "+="},
      {"*=", "*="},
      {"/=", "/="},
      {"-=", "-="},
			{"^=", "^="},
      {"==", "=="},
      {"!=", "!="},
      {"<=", "<="},
      {">=", ">="},
      {"++", "++"},
      {"--", "--"},
      {"x|", "x|"},
      {"||", "||"},
      {"&&", "&&"},
			{"or", "||"},
    },
    {
      {"xor", "x|"},
			{"not", "!"},
      {"and", "&&"},
    }
  };

  // syscalls: the name of the function associated with the index of that syscall in the VM
  // since it's compiled we have to have "definitions" of these built in C++ functions
  inline std::map<string, int> syscalls = {
    {"output", 0},
    {"input", 1}
  };

  // built-in imports
  inline vector<string> imports = {
    "hw"
  };

  template<typename T>
	bool vincludes(std::vector<T> arr, T value) {
		return std::find(arr.begin(), arr.end(), value) != arr.end();
	}

  // These are operators with no right hand value or rvalue
  // Used in parser.cpp and parser.h
  inline vector<string> noRightHand = {
    "++",
    "--"
  };
  // Used in parser.cpp and parser.h
  inline vector<string> assignments = {
    "=",
    "++",
    "--",
    "+=",
    "-=",
		"*=",
		"/=",
		"%=", //modulus
		"^=", //exponet
		"|=" // assigns if left is none, else does nothing not implemented yet
  };

  // Order of operations. Precedence is defined by the lower value
  // higer values are stronger than lower values 
  // Precedence is used in parser.cpp and parser.h
  inline std::map<string, int> precedence = {
    {"=",  1},
    {"+=", 2},
    {"-=", 2},
    {"*=", 2},
    {"/=", 2},
    {"|=", 2},
		{"^=", 2},
    {"||", 4},
    {"or", 4},
		{"x|", 4},
		{"xor", 4},
		{"and", 5},
    {"&&", 5},
    {"++", 6},
    {"--", 6},
    {"<",  7},
    {">",  7},
    {">=", 7},
    {"<=", 7},
    {"==", 7},
    {"!=", 7},
    {"+",  10},
    {"-",  10},
    {"*",  20},
		{"^",  20},// also all function are defined in lexer.h and token.h
    {"/",  20}, // it checks in lexer.cpp near the top and does the tokenizing in tokenize
    {"%",  20}, // Try it and the idea might come in my head and ill be able to help maybe
		{"//",  11}//Where is the code that checks for the symbols?
  };

  // Keywords
  // used in lexer.cpp and lexer.h
  inline std::map<string, TokenTypes> reservedWords = {
    {"funct", TokenTypes::Keyword},
    {"return", TokenTypes::Keyword},
    {"class", TokenTypes::Keyword},
    {"for", TokenTypes::Keyword},
    {"while", TokenTypes::Keyword},
    {"if", TokenTypes::Keyword},
		{"is", TokenTypes::Keyword},
    {"else", TokenTypes::Keyword},
		{"match", TokenTypes::Keyword}, //works like switch
		{"case", TokenTypes::Keyword},
		{"default", TokenTypes::Keyword},
    {"true", TokenTypes::Boolean},
    {"false", TokenTypes::Boolean},
		{"none", TokenTypes::None},
		{"null", TokenTypes::None},//We should have either none, null, or both
    {"as", TokenTypes::Keyword},
		{"in", TokenTypes::Keyword},
    {"static", TokenTypes::Keyword},
    {"break", TokenTypes::Keyword},
    {"continue", TokenTypes::Keyword},
		{"reference", TokenTypes::Keyword},
		{"private", TokenTypes::Keyword},
		{"await", TokenTypes::Keyword},
		{"getter", TokenTypes::Keyword},
  };
}; // namespace Aardvark

#endif