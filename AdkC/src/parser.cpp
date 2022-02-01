#include "../include/parser.h"

namespace Aardvark {
  using std::vector;
  using std::string;

  AST::AST() {
    type = ASTTypes::None;
    value = Token();
  }

  AST::AST(Token value) {
    type = ASTTypes::None;
    this->value = value;
  }

  AST::AST(ASTTypes type, Token value) {
    this->type = type;
    this->value = value;
  }

  string AST::toString() {
    string str = "AST<";
    str += std::to_string((int)type);
    str += ", ";
    str += value.toString();
    str += ">";

    return str;
  }

  Parser::Parser() {
    this->index = 0;
    curTok = Token();
  }

  Parser::Parser(vector<Token> tokens) {
    this->tokens = tokens;
    curTok = this->tokens.at(0);
  }

  Token Parser::advance(int amt = 1) {
    index += amt;
    if (index > tokens.size() - 1) {
      return curTok = Token();
    }

    return curTok = tokens.at(index);
  }

  Token Parser::peek(int amt = 1) {
    if (index + amt > tokens.size() - 1) return Token();
    return tokens.at(index + amt);
  }

  bool Parser::isIgnore(Token tok) {
    return tok.equals(TokenTypes::Delimiter, ";") || tok.equals("\\n") || tok.equals(TokenTypes::Linebreak);
  }

  bool Parser::isEOF() {
    if (curTok.isNull()) return true;

    return curTok.equals(TokenTypes::EndOfFile);
  }

  void Parser::skipIgnore() {
    while (isIgnore(curTok) && !curTok.isNull() && !isEOF()) {
      advance();
    }
  }

  void Parser::skipLinebreak() {
    while (curTok.equals(TokenTypes::Linebreak) || curTok.equals("\\n")) {
      advance();
    }
  }

  void Parser::skipOver(string val, Token tok) {
    if (!tok.equals(val)) throw "fail";

    advance();
  }

  void Parser::skipOver(TokenTypes type, Token tok) {
    if (!tok.equals(type)) throw "fail";

    advance();
  }

  void Parser::skipOver(TokenTypes type, string val, Token tok) {
    if (!tok.equals(type, val)) throw "fail";

    advance();
  }

  bool Parser::isBlockMerger(Token tok) {
    return (
      tok.equals(TokenTypes::Keyword, "while") ||
      tok.equals(TokenTypes::Keyword, "if") ||
      tok.equals(TokenTypes::Keyword, "else")
    );
  }

  vector<AST*> Parser::pDelimiters(string start, string end, string separator = "") {
    vector<AST*> values = {};
    bool isFirst = true;

    skipOver(start, curTok);
    skipLinebreak();

    while (!isEOF()) {
      if (curTok.equals(TokenTypes::Delimiter, end)) {
        break;
      } else if (isFirst) {
        isFirst = false;
      } else {
        if (separator == "") {
          skipIgnore();
          skipLinebreak();
        } else {
          skipOver(separator, curTok);
          skipLinebreak();
        }
      }

      if (curTok.equals(TokenTypes::Delimiter, end)) {
        break;
      }

      AST* val = pExpression();
      values.push_back(val);
      skipLinebreak();
    }
    
    skipLinebreak();
    if (isIgnore(curTok)) {
      skipIgnore();
    }
    skipLinebreak();
    skipOver(end, curTok);

    return values;
  }

  bool Parser::isCallable(AST* callStmt) {
    return (
      callStmt->type != ASTTypes::Function
      && callStmt->type != ASTTypes::If
      && callStmt->type != ASTTypes::Return
    );
  }

  AST* Parser::checkCall(AST* expr) {
    if (
      peek().equals(TokenTypes::Delimiter, "(")
      && isCallable(expr)
      && curTok.equals(TokenTypes::Identifier)
    ) {
      return pCall(expr);
    }

    return expr;
  }

  AST* Parser::pCall(AST* expression) {
    Token varName = expression->value;
    advance();

    AST* funcCall = new AST(ASTTypes::FunctionCall, varName);
    funcCall->args = pDelimiters("(", ")", ",");

    // pDotOp(funcCall);

    // funcCall.isCall = true;

    return funcCall;
  }

  AST* Parser::pBinary(AST* left, int prec) {
    Token op = curTok;

    if (op.equals(TokenTypes::Operator)) {
      string opval = op.toString();
      int newPrec = precedence.at(opval);

      if (newPrec > prec) {
        advance();

        ASTTypes type = vincludes(assignments, opval)
          ? ASTTypes::Assign
          : ASTTypes::Binary;
        
        AST* expr = new AST(type, op);
        expr->left = left;
        if (left->type == ASTTypes::None) {
          if (vincludes(noRightHand, opval)) {
            expr->left = pBinary(pAll(), 0);
            expr->isOpBefore = true;
          }
        }

        expr->op = op;
        if (vincludes(noRightHand, opval)) {
          expr->right = new AST();
        } else {
          expr->right = pBinary(pAll(), newPrec);
        }

        return pBinary(expr, prec);
      }
    }

    return left;
  }

  AST* Parser::pAttribute(AST* expr) {
    expr->type = ASTTypes::Function;
    expr->value = curTok;

    advance();

    if (curTok.equals("(")) {
      expr->args = pDelimiters("(", ")", ",");
    }

    expr->block = pDelimiters("{", "}");

    return expr;
  }

  AST* Parser::pAccess(AST* expr) {
    if (!curTok.equals(".") && !curTok.equals("[")) {
      return expr;
    }

    bool isSubscript = curTok.equals("[");
    advance();

    if (
      curTok.equals(TokenTypes::Identifier)
      || (
        isSubscript && (
          curTok.equals(TokenTypes::String)
          || curTok.equals(TokenTypes::Int)
        )
      )
    ) {
      AST* access = pExpression();
      if (isSubscript) {
        access->isSubscript = true;
        skipOver("]", curTok);

        if (vincludes(assignments, curTok.toString())) {
          access = pBinary(access, 0);
        }

        pAccess(access);

        if (
          access->type == ASTTypes::String
          || access->type == ASTTypes::Int
        ) {
          access->parent = expr;
          expr->access = access;
          return expr;
        }
      }

      if (
        access->type != ASTTypes::FunctionCall
        && access->type != ASTTypes::Identifier
        && access->type != ASTTypes::Assign
      ) {
        throw "Unable to access with that is not identifier or call!"; // Todo: Fix 'error'
      }

      access->parent = expr;
      expr->access = access;
    }

    return expr;
  }

  AST* Parser::pIdentifier(AST* expr) {
    expr->type = ASTTypes::Identifier;

    if (expr->value.toString()[0] == '~')
      return pAttribute(expr);

    if (!peek().equals(TokenTypes::Delimiter, "(")) {
      advance();
    }

    pAccess(expr);

    return expr;
  };

  AST* Parser::pFunction(bool isStatic = false) {
    if (isStatic) {
      advance();
    }

    advance(); // skip over 'funct'

    Token name = curTok;
    advance();

    AST* funcStmt = new AST(ASTTypes::Function, name);
    funcStmt->args = pDelimiters("(", ")", ",");
    funcStmt->block = pDelimiters("{", "}");
    funcStmt->isStatic = isStatic;

    return funcStmt;
  };

  AST* Parser::pReturn() {
    Token retToken = curTok;
    advance(); // skip over 'return'

    AST* retStmt = new AST(ASTTypes::Return, retToken);
    
    if (!curTok.equals(TokenTypes::Delimiter) && !curTok.equals(TokenTypes::Linebreak)) {
      retStmt->assign = pExpression();
    }

    return retStmt;
  }

  AST* Parser::pClass() {
    advance(); // skip over 'class'

    Token name = curTok;
    advance();

    AST* cls = new AST(ASTTypes::Class, name);
    cls->block = pDelimiters("{", "}");

    return cls;
  }

  AST* Parser::pWhile() {
    AST* whileStmt = new AST(ASTTypes::While, curTok);
    advance(); // skip over 'while'

    whileStmt->condition = pExpression();
    whileStmt->block = pDelimiters("{", "}");

    return whileStmt;
  }

  AST* Parser::pInclude() {
    Token file = curTok;
    advance(); // skip over #include 'file' -> the file part

    AST* include = new AST(ASTTypes::Include, file);

    if (curTok.equals(TokenTypes::Keyword, "as")) {
      advance(); // skip over 'as'

      include->as = new AST(ASTTypes::Identifier, curTok);
      advance(); // skip over 'identifier'
    }

    return include;
  }

  AST* Parser::pDirective() {
    Token directiveType = curTok;
    advance(); // skip over first part of directive eg: (#include -> "include")

    if (directiveType.toString() == "include") {
      return pInclude();
    }
		//add #define and #jump.

    return new AST(); // throw error here or maybe act as comment idk
  }

  AST* Parser::pIf() {
    Token ifTok = curTok; // just 'if' keyword token

    advance(); // skip over 'if'

    AST* expr = pExpression();

    AST* ifStmt = new AST(ASTTypes::If, ifTok);
    ifStmt->condition = expr;

    if (isBlockMerger(curTok)) {
      ifStmt->block.push_back($pAll());
    } else {
      ifStmt->block = pDelimiters("{", "}");
    }

    if (curTok.equals(TokenTypes::Keyword, "else")) {
      Token elseTok = curTok;

      advance(); // skip over 'else'

      AST* elsStmt = new AST(ASTTypes::Else, elseTok);

      if (isBlockMerger(curTok)) {
        elsStmt->block.push_back($pAll());
      } else {
        elsStmt->block = pDelimiters("{", "}");
      }

      ifStmt->els = elsStmt;
    }

    return ifStmt;
  }

  // The main parsing branch if all else fails
  // meaning if it didn't already parse a function call or binary expression
  // anything like classes, function, variables, loops, and if statements will
  // be parsed here
  AST* Parser::$pAll() {
    if (curTok.equals(TokenTypes::Delimiter, "(")) {
      advance();
      AST* expr = pExpression();
      skipOver(TokenTypes::Delimiter, ")", curTok);

      return expr;
    }

    AST* oldTok = new AST(curTok);

    // Directives
    if (curTok.equals(TokenTypes::Directive)) {
      return pDirective();
    }

    // Methods / Functions
    if (curTok.equals(TokenTypes::Keyword, "funct")) {
      return pFunction();
    } else if (curTok.equals(TokenTypes::Keyword, "static")) {
      return pFunction(true);
    }

    if (curTok.equals(TokenTypes::Keyword , "class")) {
      return pClass();
    }

    // Branches
    if (curTok.equals(TokenTypes::Keyword, "if")) {
      return pIf();
    }

    // Loops
    if (curTok.equals(TokenTypes::Keyword , "while")) {
      return pWhile();
    }

    // Loop Breaks
    if (curTok.equals(TokenTypes::Keyword , "break")) {
      oldTok->type = ASTTypes::Break;
      advance();

      return oldTok;
    } else if (curTok.equals(TokenTypes::Keyword , "continue")) {
      oldTok->type = ASTTypes::Continue;
      advance();

      return oldTok;
    } else if (curTok.equals(TokenTypes::Keyword, "return")) {
      return pReturn();
    }

    // Literals
    if (curTok.equals(TokenTypes::Boolean)) {
      oldTok->type = ASTTypes::Boolean;
      advance();

      return oldTok;
    } else if (curTok.equals(TokenTypes::String)) {
      oldTok->type = ASTTypes::String;

      advance();

      return oldTok;
    } else if (curTok.equals(TokenTypes::Int)) {
      oldTok->type = ASTTypes::Int;

      advance();

      return oldTok;
    } else if (curTok.equals(TokenTypes::Double)) {
      oldTok->type = ASTTypes::Double;

      advance();

      return oldTok;
    }

    // Other
    if (curTok.equals(TokenTypes::Identifier)) {
      return pIdentifier(oldTok);
    }

    if (curTok.equals(TokenTypes::Linebreak, "\\n")) {
      while (curTok.equals(TokenTypes::Linebreak, "\\n")) {
        advance();
      }

      return pAll();
    }

    if (isIgnore(curTok)) {
      skipIgnore();
    }

    return oldTok;
  }

  // always call pAll() first when trying to call $pAll() because
  // it checks for a function call first
  AST* Parser::pAll() {
    return checkCall($pAll());
  }

  // Parses everything and checks if the first thing is a function call
  AST* Parser::pExpression() {
    return checkCall(pBinary(pAll(), 0));
  }

  // loops through the entire top node of the tree
  AST* Parser::parse() {
    AST* ast = new AST(ASTTypes::Scope, Token(TokenTypes::Identifier, "_GLOBAL_"));
    curTok = this->tokens[0];

    while (!curTok.isNull() && !isEOF()) {
      AST* exp = pExpression();
      ast->block.push_back(exp);

      skipLinebreak();
      if (isIgnore(curTok)) {
        skipIgnore();
      }
      skipLinebreak();
    }
    
    return ast;
  }
}; // namespace Aardvark