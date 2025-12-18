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
  Book() {
    ISBN[0] = '\0';
    book_name[0] = '\0';
    auther[0] = '\0';
    keyword[0] = '\0';
    stock = 0;
    price = 0;
  }
  Book (const char* isbn) {
    strncpy(ISBN, isbn, ISBN_LEN);
    book_name[0] = '\0';
    auther[0] = '\0';
    keyword[0] = '\0';
    stock = 0;
    price = 0;
  }
};