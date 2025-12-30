#include "book.hpp"

Book::Book() : stock(0), price(0) {
  ISBN[0] = '\0';
  book_name[0] = '\0';
  auther[0] = '\0';
  keyword[0] = '\0';
}
Book::Book(const char *isbn) : stock(0), price(0) {
  strncpy(ISBN, isbn, ISBN_LEN);
  book_name[0] = '\0';
  auther[0] = '\0';
  keyword[0] = '\0';
}