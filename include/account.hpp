#pragma once

#include <cstring>

class Account {
private:
  static const int MAX_LEN = 35;
  char userID[MAX_LEN], password[MAX_LEN], username[MAX_LEN];
  int selected_index;
  int privilege;

public:
  Account();
  Account(const char *id, const char *pw, const char *name, int type = 1);
  void passwd(char *new_pw);
  ;
  void Delete();
  friend class Bookstore;
};