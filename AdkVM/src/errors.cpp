#include "../include/errors.h"

namespace Aardvark {

  StringBuilder::StringBuilder(string data, bool autoReset = true) {
    str = data;
    this->autoReset = autoReset;
  }

  string StringBuilder::SetRGBf(int r, int g, int b) {
    return SetRGBf(r, g, b, 0);
  }

  string StringBuilder::SetRGBf(int r, int g, int b, int index) {
    string x = "\x1b[38;2;";
    x += std::to_string(r);
    x += ";";
    x += std::to_string(g);
    x += ";";
    x += std::to_string(b);
    x += "m";

    string front = str.substr(0, index);
    string back = str.substr(index);

    str = front + x + back;

    if (this->autoReset) {
      str += "\x1b[0m";
    }

    return str; // maybe return 'this?'
  }

  string StringBuilder::GetString() {
    return this->str;
  }

  AdkError::AdkError(const string message) {
    StringBuilder strB = StringBuilder(this->msg, true);
    this->msg = strB.SetRGBf(204, 10, 10);
    this->msg += message;
  }

  void AdkError::append(const string message) {
    this->msg += message;
  }

  void AdkError::throwErr() {
    std::cerr << msg << std::endl;
    exit(1);
  }

  AdkWarning::AdkWarning(const string message) {
    StringBuilder strB = StringBuilder(this->msg, true);
    this->msg = strB.SetRGBf(117, 80, 123);
    this->msg += message;
  }

  void AdkWarning::printWarning() {
    std::cout << msg << std::endl;
  }

  void AdkWarning::throwWarning() {
    string warningMsg = "Warning thrown: ";
    warningMsg += msg.substr(9);
    
    AdkError err = AdkError(warningMsg);
    err.throwErr();
  }

}; // namespace Aardvark