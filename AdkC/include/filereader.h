#pragma once
#ifndef FILEREADER_H
#define FILEREADER_H
#include <fstream>
#include <string>
#include <iostream>

namespace Aardvark {

  std::string readFile(std::string filename);
  void writeFile(std::string filename, std::string data);

}; // namespace Aardvark

#endif