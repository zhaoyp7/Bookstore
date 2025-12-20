#pragma once

#include "account.hpp"
#include "book.hpp"
#include <cstring>
#include <iostream>

class Log {
public:
  static const int MAX_LEN = 35;
  char op[MAX_LEN];
  Account user, target;
  Book book, old_book;
  char password[MAX_LEN];
  int quantity, cost;
  int nxt;
  Log();
};