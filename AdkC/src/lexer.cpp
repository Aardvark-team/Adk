#include "../include/lexer.h"

namespace Aardvark {
  Token::Token() {
    type = TokenTypes::None;
    value = nullptr;
  }

  Token::Token(TokenTypes type): type(type) {
    value = nullptr;
  }

  Token::Token(TokenTypes type, string value): type(type) {
    this->value = new string(value);
  }

  Token::Token(TokenTypes type, char value): type(type) {
    this->value = new string(1, value);
  }

  Token::Token(int value): type(TokenTypes::Int) {
    this->value = new int(value);
  }

  Token::Token(double value): type(TokenTypes::Double) {
    this->value = new double(value);
  }

  Token::Token(bool value): type(TokenTypes::Boolean) {
    this->value = new bool(value);
  }

  void Token::updatePosition(Position pos) {
    this->pos = pos;
  }

  void Token::updatePosition(int line, int col) {
    this->pos = Position();
    this->pos.line = line;
    this->pos.col = col;
  }

  bool Token::isNull() {
    return this->type == TokenTypes::None;
  }

  string Token::toString() {
    switch(type) {
      case TokenTypes::None:
        return "NONE";

      case TokenTypes::Keyword:
      case TokenTypes::Identifier:
      case TokenTypes::Delimiter:
      case TokenTypes::Operator:
      case TokenTypes::String:
      case TokenTypes::Directive:
        return *(string*)value;
      
      case TokenTypes::Int:
        return *(string*)value;

      case TokenTypes::Double:
        return *(string*)value;

      case TokenTypes::Boolean:
        return ((*(string*)value) == "true") ? "true" : "false";

      case TokenTypes::Linebreak:
        return "Linebreak";

      default:
        return "Unknown";
    }
  }

  template<typename T>
  T Token::cast() {
    return *static_cast<T*>(this->value);
  }

  bool Token::equals(string value) {
    return this->toString() == value;
  }

  bool Token::equals(TokenTypes type, string value) {
    return this->equals(type) && this->equals(value);
  }

  bool Token::equals(TokenTypes type) {
    return this->type == type;
  }

  /* Start of Lexer */

  Lexer::Lexer() {
    this->reset();
  }

  Lexer::Lexer(string input) {
    this->reset();
    this->input = input;
    curChar = input[0];
  }

  /* Default - advance(int amt = 1) */
  char Lexer::advance() { return advance(1); }
  char Lexer::advance(int amt) {
    index += amt;
    column += amt;

    if (index >= input.size()) {
      curChar = '\0';
      return curChar;
    }
    
    curChar = input[index];
    return curChar;
  }

  /* Default - peek(int amt = 1) */
  char Lexer::peek() { return peek(1); }
  char Lexer::peek(int amt) {
    if (index + amt >= input.size()) {
      return '\0';
    }
    
    return input[index + amt];
  }

  /* Default - grab(int amt = 1) */
  /*
    "Grabs" all characters from current index + the amount specified and returns a string of those characters.

    This allows for operator checking and peeking multiple values ahead
  */
  string Lexer::grab() { return grab(1); }
  string Lexer::grab(int amt) {
    string value = "";
    value += curChar;

    for (int i = 1; i < amt; i++) {
      value += peek();
    }

    return value;
  }

  bool Lexer::isWhitespace(char c) {
    return c == ' ' || c == '\r' || c == '\t';
  }
  bool Lexer::isSpecial(char c) {
    return (
      c == '_' ||
      c == '~' ||
      c == '@' ||
      c == '$'
    );
  }

  bool Lexer::isAlpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || isSpecial(c);
  }

  bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
  }

  bool Lexer::isNumber(char c) {
    return (
      (c == '-' && isDigit(peek()))
      || isDigit(c)
    );
  }

  // Does a regular quote check
  bool Lexer::isQuote(char c) { return c == '\'' || c == '\"'; };

  // Checks the quote passed with the character
  bool Lexer::isQuote(char c, char quote) {
    if (quote != '"' && quote != '\'') return false;
    if (quote == '"')
      return c == '"';
    else if (quote == '\'')
      return c == '\'';

    return false;
  }

  // Checks if character is currently an operator
  // Checks farther out depending on the defined operators
  int Lexer::isOperator(char c) {
    for (int i = operators.size()-1; i >= 0; i--) {
      // Get the map from the operators vectir
      std::map<string, string> value = operators.at(i);

      // Loop through each key and compare
      for (auto const& [ key, val ] : value) {
        if (grab(i + 1) == key) {
          return i + 1; // Returns the length of the operator
        }
      }
    }

    return 0;
  }

  bool Lexer::isDelimiter(char c) {
    return (
      (c == '(') ||
      (c == ')') ||
      (c == '{') ||
      (c == '}') ||
      (c == '[') ||
      (c == ']') ||
      (c == ';') ||
      (c == ':') ||
      (c == ',') ||
      (c == '.')
    );
  }

  vector<Token> Lexer::tokenize(string input) {
    this->input = input;
    curChar = this->input[0];
    return tokenize();
  }
  vector<Token> Lexer::tokenize() {
    while (curChar != '\0') {
      int oldIndex = index;
      if (isWhitespace(curChar))
        advance();

      if (curChar == '\n') {
        tokens.push_back(Token(TokenTypes::Linebreak, "\\n"));
        column = 0;
        line++;

        advance();
      } 

      if (curChar == '#') { // Directives
        string value = "";
        advance();
        while (!isWhitespace(curChar) && curChar != '\n') {
          value += curChar;
          advance();
        }

        Token tok = Token(TokenTypes::Directive, value);
        tok.updatePosition(line, column);
        tokens.push_back(tok);

        advance();

        string newValue = "";
        while (!isWhitespace(curChar) && curChar != '\n') {
          newValue += curChar;
          advance();
        }

        Token tok2 = Token(TokenTypes::String, newValue);
        tok2.updatePosition(line, column);
        tokens.push_back(tok2);
      }

      // Comments
      if (curChar == '/' && peek() == '/') {
        advance();
        advance();
        if (isWhitespace(curChar) && peek() == '\n') advance();
        //Just incase they put a space after the //
        if (curChar == '\n') {
          advance();
          while (curChar != '\\' || peek() != '\\') {
            advance();
          }
          advance();
          advance();
        } else {
          while (curChar != '\n' && curChar != '\0') {
            advance();
          }
        }
        
      }

      if (isOperator(curChar) > 0) {
        int opLength = isOperator(curChar);
        string value = grab(opLength);
        string opValue = operators[opLength - 1][value];

        Token tok = Token(TokenTypes::Operator, opValue);
        tok.updatePosition(line, column);
        tokens.push_back(tok);
        advance(opLength);
      }

      if (isDelimiter(curChar)) {
        Token tok = Token(TokenTypes::Delimiter, curChar);
        tok.updatePosition(line, column);

        advance();

        tokens.push_back(tok);
      }

      if (isNumber(curChar)) {
        TokenTypes type = TokenTypes::Int;
        int col = column;
        int ln = line;

        string val = "";

        if (curChar == '-') {
          val += curChar;
          advance();
        }

        while (isNumber(curChar)) {
          val += curChar;
          advance();

          if (curChar == '.') {
            type = TokenTypes::Double;
            val += ".";

            advance();
          }
        }

        Token tok = Token(type, val);
        tok.updatePosition(ln, col);

        tokens.push_back(tok);
      }

      if (isQuote(curChar)) {
        char quote = curChar;
        int col = column;
        int ln = line;

        string val = "";
        advance();

        while (curChar != '\0' && curChar != quote) {
          if (curChar == '\n') {
            // throw new Error("SyntaxError: Unexpected characater \\n");
            std::cout << "SyntaxError: Unexpected character '\\n'" << "\n";
            return this->tokens; // TODO: ERROR
          };
          val += curChar;
          advance();
        }

        advance();

        Token tok = Token(TokenTypes::String, val);
        tok.updatePosition(ln, col);

        tokens.push_back(tok);
      }

      if (isAlpha(curChar)) {
        string val = "";

        int col = column;
        int ln = line;

        // TODO: FIX Captures things like x-2 as a whole token
        while (curChar != '\0' && (isAlpha(curChar) || isNumber(curChar))) {
          val += curChar;
          advance();
        }

        TokenTypes type = reservedWords.find(val) != reservedWords.end()
          ? reservedWords.at(val)
          : TokenTypes::Identifier;

        // System.out.println(type);

        Token tok = Token(type, val);
        tok.updatePosition(ln, col);

        tokens.push_back(tok);
      }

      if (oldIndex == index) {
        // System.out.println("Error: Unknown character " + curChar);
        std::cout << "Error: Unknown character " << curChar << "\n"; 
        return tokens;
      }
    }

    return tokens;
  }

  void Lexer::reset() {
    index = 0;
    line = 1;
    column = 1;

    curChar = '\0';
    input = "";
		// Later add or, and, xor, and not
  }
}; // namespace Aardvark