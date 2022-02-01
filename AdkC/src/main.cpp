#include <filesystem>
#include "../include/compiler.h"

namespace fs = std::filesystem;
using namespace Aardvark;

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: adkc <inputfile>\n";
    return 1;
  }

  fs::path executablePath = argv[0];
  fs::path executableFullPath = fs::current_path() / fs::absolute(executablePath);
  fs::path adkFile = argv[1];
  fs::path fullFile = fs::current_path() / fs::absolute(adkFile);

  string filepath = fullFile.string();

  if (filepath.find_last_of(".adk") == string::npos) {
    std::cerr << "Usage: adkc <inputfile>\n  File must be an Aardvark file (adk).";
    return 1;
  }

  string newfile = (filepath.substr(0, filepath.find_last_of(".adk") - 3));
  newfile += ".adkb";

  string data = readFile(filepath);

  Lexer lexer = Lexer(data);
  auto tokens = lexer.tokenize();

  Parser parser = Parser(tokens);
  AST* ast = parser.parse();

  Compiler compiler = Compiler(ast);
  compiler.currentFile = filepath;
  string code = compiler.compile();

  if (argc >= 3 && std::string(argv[2]) == "-co") {
    std::cout << code << std::endl;
  } else {
    writeFile(newfile, code);
  }

  return 0;
}