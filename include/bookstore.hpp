#include "BlockLinkedList.hpp"
#include "MemoryRiver.hpp"
#include "account.hpp"
#include "book.hpp"
#include "command.hpp"
#include "log.hpp"
#include <cstring>
#include <map>
#include <string>
#include <vector>

inline bool cmpISBN(const Book &a, const Book &b) {
  return strcmp(a.ISBN, b.ISBN) < 0;
}

class Bookstore {
public:
  MemoryRiver<Account> account_data; // info1: root's index
  BlockLinkedList account_block;

  MemoryRiver<Book> book_data;
  BlockLinkedList book_ISBN_block;
  BlockLinkedList book_name_block;
  BlockLinkedList book_author_block;
  BlockLinkedList book_key_block;

  MemoryRiver<Finance> finance_data;
  // info1: last index
  // info2: Total Count

  MemoryRiver<Log> log_data;
  // info1: head index
  // info2: last index

  Command command;
  Account now_user;
  std::map<std::string, int> login_state;
  std::vector<Account> login_stack;
  Bookstore()
      : account_data("account_data"), account_block("account_block"),
        book_data("book_data"), book_ISBN_block("book_ISBN_block"),
        book_name_block("book_name_block"),
        book_author_block("book_author_block"),
        book_key_block("book_key_block"), finance_data("finance_data"),
        log_data("log_data") {}

  void init();
  void Exit();
  void Su();
  void Logout();
  void Register();
  void Passwd();
  void Useradd();
  void Delete();
  void Buy();
  void Select();
  void Import();
  void CheckParameter(std::string &str, int &type);
  void UpdateKeyBlock(const std::string &str, int index, int op);
  void GetBookInformation(std::string &ISBN, std::string &name,
                          std::string &author, std::string &key,
                          std::string &price_str, int op);
  void Modify();
  int KeywordNumber(const std::string &str);
  bool FindKeyword(const Book &book, const std::string &key);
  void ShowBook();
  void ShowFinance();
  void Show();
  void ReportFinance();
  void ReportEmployee();
  void Report();
  void ShowLog();
  void main();
};