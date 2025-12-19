#include <iostream>
#include <cstring>

class Account {
// private:
public:
  static const int MAX_LEN = 35;
  char userID[MAX_LEN], password[MAX_LEN], username[MAX_LEN];
  int selected_index;
  int privilege;
public:
  Account() {
    userID[0] = '\0';
    password[0] = '\0';
    username[0] = '\0';
    selected_index = -1;
    privilege = 0;
  }
  Account(const char* id, const char* pw, const char* name, int type = 1) {
    strncpy(userID, id, MAX_LEN);
    strncpy(password, pw, MAX_LEN);
    strncpy(username, name, MAX_LEN);
    selected_index = -1;
    privilege = type;
  }
  int get_type() {return privilege;}
  void passwd(char* new_pw) {
    strncpy(password, new_pw, MAX_LEN);
  }
  void Delete() {}
};