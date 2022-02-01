#include <iostream>
#include <string>

namespace Aardvark {
  using std::string;

  class StringBuilder {
    public:

    StringBuilder(string data, bool autoReset);

    string SetRGBf(int r, int g, int b); // default index = 0
    string SetRGBf(int r, int g, int b, int index);

    string GetString();

    private:

    bool autoReset = true;
    string str = "";
  };

  class AdkError {
    public:
    string msg = "Error: ";

    AdkError(const string message);

    void append(const string message);
    
    void throwErr();
  };

  class AdkWarning {
    public:
    string msg = "Warning: ";

    AdkWarning(const string message);

    void printWarning();
    void throwWarning();
  };

}; // namespace Aardvark