#include "../include/filereader.h"

namespace Aardvark {

  std::string readFile(std::string filename) {
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

      data += line;
    }

    return data;
  }

  void writeFile(std::string filename, std::string data) {
    std::ofstream file(filename);

    file << data;

    file.close();
  }

}; // namespace Aardvark