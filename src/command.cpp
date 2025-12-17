#include <iostream>
#include <cstring>

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
};