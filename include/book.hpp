#pragma once

#include <cstring>

class Book {
// private:
public:
  static const int ISBN_LEN = 25;
  static const int MAX_LEN = 65;
  char ISBN[ISBN_LEN];
  char book_name[MAX_LEN], auther[MAX_LEN];
  char keyword[MAX_LEN];
  double price;
  int stock;
public:
  Book();
  Book (const char* isbn);
};