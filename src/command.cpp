#include "command.hpp"

bool Command::read() {
  bool res = (bool)std::getline(std::cin, str);
  sz = str.size();
  pos = 0;
  return res;
}
std::string Command::getstr() {
  std::string ans = "";
  while (pos < sz && str[pos] == ' ') {
    pos++;
  }
  while (pos < sz && str[pos] != ' ') {
    ans += str[pos++];
  }
  return ans;
}
void Command::moveback() {
  while (pos > 0 && str[pos - 1] != ' ') {
    pos--;
  }
}