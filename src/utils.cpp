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
    }
  }
  if (!CheckLegalString(str.data())) {
    return false;
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
    }
  }
  if (!CheckLegalString(str.data())) {
    return false;
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

bool CheckLegalString(const char* str) {
  int sz = strlen(str);
  int pos = 0;
  while (pos < sz) {
    unsigned char c = str[pos];
    int len = 0;
    if (c <= 0x7F) { //0xxxxxxx
      len = 1;
    } else if (c >> 5 == 0x06) { // 110xxxxxxx
      len = 2;
    } else if (c >> 4 == 0x0E) { // 1110xxxx
      len = 3;
    } else if (c >> 3 == 0x0F) { // 11110xxx
      len = 4;
    } else {
      return false;
    }
    if (len == 1) {
      if (str[pos] < 32 || str[pos] > 126) {
        return false;
      }
      pos++;
      continue;
    }
    if (pos + len > sz) {
      return false;
    }
    for (int i = 1; i < len; i++) {
      unsigned char ch = str[pos + i];
      if (ch >> 6 != 0x02) {
        return false;
      }
    }
    unsigned int value = 0;
    if (len == 2) {
      value = ((str[pos] & 0x1F) << 6) | (str[pos + 1] & 0x3F);
    } else if (len == 3) {
      value = ((str[pos] & 0x0F) << 12) | ((str[pos + 1] & 0x3F) << 6) | (str[pos + 1] & 0x3F);
    } else if (len == 4) {
      value = ((str[pos] & 0x07) << 18) | ((str[pos + 1] & 0x3F) << 12) | ((str[pos + 1] & 0x3F) << 6) | (str[pos + 1] & 0x3F);
    }

    bool flag = 0;
    flag |= (value >= 0x4E00 && value <= 0x9FFF);
    flag |= (value >= 0x3400 && value <= 0x4DBF);
    flag |= (value >= 0x20000 && value <= 0x2A6DF);
    flag |= (value >= 0x2A700 && value <= 0x2B73F);
    flag |= (value >= 0x2B740 && value <= 0x2B81F);
    flag |= (value >= 0x2B820 && value <= 0x2CEAF);
    flag |= (value >= 0x2CEB0 && value <= 0x2EBEF);
    flag |= (value >= 0x30000 && value <= 0x3134F);
    flag |= (value >= 0x31350 && value <= 0x323AF);
    flag |= (value >= 0xF900 && value <= 0xFAFF);
    flag |= (value >= 0x2F800 && value <= 0x2FA1F);
    flag |= (value == 0x00B7);
    flag |= (value == 0x2014);
    flag |= (value == 0xFF08);
    flag |= (value == 0xFF09);
    flag |= (value == 0xFF1A);
    if (!flag) {
      return false;
    }
    pos += len;
  }
  return true;
}