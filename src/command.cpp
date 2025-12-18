#include <cstring>
#include <iostream>

class Invalid {
private:
  const char *message;

public:
  Invalid() : message("Invalid"){};
  const char *what() const { return message; }
};

class Command {
private:
  int sz, pos;
  std::string str;

public:
  bool read() {
    std::getline(std::cin, str);
    if (std::cin.eof()) {
      return false;
    }
    sz = str.size();
    pos = 0;
    return true;
  }
  std::string getstr() {
    std::string ans = "";
    while (pos < sz && str[pos] == ' ') {
      pos++;
    }
    while (pos < sz && str[pos] != ' ') {
      ans += str[pos++];
    }
    return ans;
  }
  void moveback() {
    while (pos > 0 && str[pos - 1] != ' ') {
      pos--;
    }
  }
};