#include "book.hpp"

Book::Book() {
  ISBN[0] = '\0';
  book_name[0] = '\0';
  auther[0] = '\0';
  keyword[0] = '\0';
  stock = 0;
  price = 0;
}
Book::Book(const char *isbn) {
  strncpy(ISBN, isbn, ISBN_LEN);
  book_name[0] = '\0';
  auther[0] = '\0';
  keyword[0] = '\0';
  stock = 0;
  price = 0;
}