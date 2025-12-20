#pragma once

#include "account.hpp"
#include "book.hpp"
#include <cstring>
#include <iostream>

struct Finance {
  static const int ISBN_LEN = 25;
  static const int MAX_LEN = 35;
  double income, expenditure;
  char ISBN[ISBN_LEN];
  char userID[MAX_LEN];
  int pre;
  Finance();
  Finance(double in, double out, char *isbn, char *id, int pr = 0);
};
class Log {
private:
  static const int MAX_LEN = 35;
  char op[MAX_LEN];
  Account user, target;
  Book book, old_book;
  char password[MAX_LEN];
  int quantity, cost;
  int nxt;

public:
  Log();
  friend class Bookstore;
};