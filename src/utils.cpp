#include "utils.hpp"

bool CheckUserID(const std::string &str) {
  int sz = str.size();
  if (sz > 30 || sz == 0) {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    bool flag = 0;
    flag |= (str[i] >= '0' && str[i] <= '9');
    flag |= (str[i] >= 'a' && str[i] <= 'z');
    flag |= (str[i] >= 'A' && str[i] <= 'Z');
    flag |= (str[i] == '_');
    if (!flag) {
      return false;
    }
  }
  return true;
}
bool CheckPassward(const std::string &str) { return CheckUserID(str); }
bool CheckPrivilege(const std::string &str) {
  int sz = str.size();
  if (sz > 1 || sz == 0) {
    return false;
  }
  return (str[0] == '1' || str[0] == '3' || str[0] == '7');
}
bool CheckUsername(const std::string &str) {
  int sz = str.size();
  if (sz == 0) {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    if (str[i] < 32 || str[i] > 126) {
      return false;
    }
  }
  return true;
}
bool CheckISBN(const std::string &str) {
  int sz = str.size();
  if (sz > 20 || sz == 0) {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    if (str[i] < 32 || str[i] > 126) {
      return false;
    }
  }
  return true;
}
bool CheckBookName(const std::string &str) {
  int sz = str.size();
  if (sz > 60 || sz == 0) {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    if (str[i] == '\"') {
      return false;
    } else if (str[i] < 32 || str[i] > 126) {
      return false;
    }
  }
  return true;
}
bool CheckAuthor(const std::string &str) { return CheckBookName(str); }
bool CheckKeyword(const std::string &str) {
  int sz = str.size();
  if (sz > 60 || sz == 0 || str[0] == '|' || str[sz - 1] == '|') {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    if (str[i] == '\"') {
      return false;
    } else if (str[i] < 32 || str[i] > 126) {
      return false;
    }
  }
  std::vector<std::string> key;
  std::string tmp = "";
  for (int i = 0; i < sz; i++) {
    if (str[i] == '|') {
      key.push_back(tmp);
      tmp = "";
    } else {
      tmp += str[i];
    }
  }
  key.push_back(tmp);
  int key_num = key.size();
  std::sort(key.begin(), key.end());
  for (int i = 0; i < key_num; i++) {
    if (key[i] == "") {
      return false;
    }
  }
  for (int i = 0; i + 1 < key_num; i++) {
    if (key[i] == key[i + 1]) {
      return false;
    }
  }
  return true;
}
bool CheckQuantity(const std::string &str) {
  int sz = str.size();
  if (sz > 10 || sz == 0) {
    return false;
  } else if (sz > 1 && str[0] == '0') {
    return false;
  }
  long long sum = 0;
  for (int i = 0; i < sz; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return false;
    }
    sum = sum * 10 + str[i] - '0';
  }
  if (sum > 2147483647) {
    return false;
  }
  return true;
}
bool CheckPrice(const std::string &str) {
  int sz = str.size();
  if (sz > 13 || sz == 0) {
    return false;
  }
  int dot_num = 0, dot_pos = 0;
  for (int i = 0; i < sz; i++) {
    if (str[i] == '.') {
      dot_num++;
      dot_pos = i;
    } else if (str[i] < '0' || str[i] > '9') {
      return false;
    }
  }
  if (dot_num > 1) {
    return false;
  } else if (dot_num == 1 && (dot_pos == 0 || dot_pos == sz - 1)) {
    return false;
  } else if (dot_num == 1 && dot_pos != 1 && str[0] == '0') {
    return false;
  }
  return true;
}
bool CheckCost(const std::string &str) { return CheckPrice(str); }
bool CheckCount(const std::string &str) { return CheckQuantity(str); }

double StringToDouble(const std::string &str) {
  int sz = str.size(), flag = 1, pos = 0;
  double ans = 0, val = 1;
  bool point = 0;
  while (pos < sz) {
    if (str[pos] == '.') {
      point = 1;
    } else if (point == 0) {
      ans = ans * 10 + str[pos] - '0';
    } else if (point == 1) {
      val *= 0.1;
      ans += (str[pos] - '0') * val;
    }
    pos++;
  }
  return ans;
}
int StringToInt(const std::string &str) {
  int ans = 0, sz = str.size();
  for (int i = 0; i < sz; i++) {
    ans = ans * 10 + str[i] - '0';
  }
  return ans;
}