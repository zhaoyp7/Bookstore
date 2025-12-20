#include "MemoryRiver.hpp"
#include "BlockLinkedList.hpp"
#include "account.hpp"
#include "book.hpp"
#include "command.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

MemoryRiver<Account> account_data("account_data"); // info1: root's index
BlockLinkedList account_block("account_block");

MemoryRiver<Book> book_data("book_data");
BlockLinkedList book_ISBN_block("book_ISBN_block");
BlockLinkedList book_name_block("book_name_block");
BlockLinkedList book_author_block("book_author_block");
BlockLinkedList book_key_block("book_key_block");

struct Finance {
  static const int ISBN_LEN = 25;
  static const int MAX_LEN = 35;
  double income, expenditure;
  char ISBN[ISBN_LEN];
  char userID[MAX_LEN];
  int pre;
  Finance() {
    income = 0;
    expenditure = 0;
    ISBN[0] = '\0';
    userID[0] = '\0';
    pre = 0;
  }
  Finance(double in, double out, char* isbn, char* id, int pr = 0) {
    income = in;
    expenditure = out;
    strncpy(ISBN, isbn, ISBN_LEN);
    strncpy(userID, id, MAX_LEN);
    pre = pr;
  } 
};
MemoryRiver<Finance> finance_data("finance_data");
// info1: last index
// info2: Total Count

MemoryRiver<Log> log_data("log_data");
// info1: head index
// info2: last index
Command command;
Account now_user;
std::map<std::string, int> login_state;
std::vector<Account> login_stack;

bool cmpISBN(const Book &a, const Book &b) {
  return strcmp(a.ISBN, b.ISBN) < 0;
}

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
void ReportFinance() {
  if (login_stack.empty() || now_user.privilege < 7) {
    throw Invalid();
  } else if (command.getstr() != "") {
    throw Invalid();
  }
  int count;
  double all_income = 0, all_expenditure = 0;
  finance_data.get_info(count, 2);
  int index;
  finance_data.get_info(index, 1);
  Finance tmp;
  std::vector <Finance> ans;
  while (count--) {
    finance_data.read(tmp, index);
    ans.push_back(tmp);
    index = tmp.pre;
  }
  int sz = ans.size();
  printf("Order\tOperation\tUserID\tISBN\tIncome\tExpenditure\n");
  for (int i = sz - 1; i >= 0; i--) {
    printf("%d\t",sz - i);
    Finance tmp = ans[i];
    if (std::abs(tmp.expenditure - 0) < 0.0000001) {
      printf("Buy");
    } else {
      printf("Import");
    }
    std::cout << '\t' << tmp.userID << '\t' << tmp.ISBN;
    printf("\t%.2lf\t%.2lf\n", tmp.income, tmp.expenditure);
    all_income += tmp.income;
    all_expenditure += tmp.expenditure;
  }
  printf("Totally Income=%.2lf Expenditure=%.2lf\n", all_income, all_expenditure);
}
void ReportEmployee() {

}
void Report() {
  std::string op = command.getstr();
  if (command.getstr() != "") {
    throw Invalid();
  } else if (op == "finance") {
    ReportFinance();
  } else if (op == "employee") {
    ReportEmployee();
  } else {
    throw Invalid();
  }
}
void ShowLog() {
  if (login_stack.empty() || now_user.privilege < 7) {
    throw Invalid();
  } else if (command.getstr() != "") {
    throw Invalid();
  }

  std::vector <Log> ans;
  int count = 0, index;
  log_data.get_info(index, 1);
  Log log;
  log_data.read(log, index);
  index = log.nxt;
  while (index) {
    log_data.read(log, index);
    index = log.nxt;
    std::string op = log.op;
    std::cout << op << '\t';
    if (op == "Su") {
      std::cout << log.user.userID << '\n';
    } else if (op == "Logout") {
      std::cout << log.user.userID << '\n';
    } else if (op == "Register") {
      std::cout << log.target.userID << '\t' << log.target.privilege << '\t';
      std::cout << "operator:" << log.user.userID << '\n';
    } else if (op == "Password") {
      std::cout << log.target.userID << '\t' << log.password << '\t';
      std::cout << "operator:" << log.user.userID << '\n';
    } else if (op == "Useradd") {
      std::cout << log.target.userID << '\t' << log.target.privilege << '\t';
      std::cout << "operator:" << log.user.userID << '\n';
    } else if (op == "Delete") {
      std::cout << log.target.userID << '\t' ;
      std::cout << "operator:" << log.user.userID << '\n';
    } else if (op == "Buy") {
      std::cout << log.book.ISBN << "\tquantity:" << log.quantity << '\t';
      std::cout << "operator:" << log.user.userID << '\n';
    } else if (op == "Import") {
      std::cout << log.book.ISBN <<"\tquantity:" << log.quantity << "\tcost:" << log.cost << '\t';
      std::cout << "operator:" << log.user.userID << '\n';
    } else if (op == "Modify") {
      puts("");
      printf("old book :");
      std::cout << log.old_book.ISBN << '\t' << log.old_book.book_name << '\t' << log.old_book.auther
              << '\t' << log.old_book.keyword;
      printf("\t%.2lf\t", log.old_book.price);
      std::cout << log.old_book.stock << '\n';
      printf("new book :");
      std::cout << log.book.ISBN << '\t' << log.book.book_name << '\t' << log.book.auther
              << '\t' << log.book.keyword;
      printf("\t%.2lf\t", log.book.price);
      std::cout << log.book.stock << '\n';
    }
  }
}

int main() {
  init();
  while (command.read()) {
    std::string op = command.getstr();
    // std::cout << "op = " << op << '\n';
    try {
      if (op == "su") {
        Su();
      } else if (op == "logout") {
        Logout();
      } else if (op == "register") {
        Register();
      } else if (op == "passwd") {
        Passwd();
      } else if (op == "useradd") {
        Useradd();
      } else if (op == "delete") {
        Delete();
      } else if (op == "show") {
        Show();
      } else if (op == "buy") {
        Buy();
      } else if (op == "select") {
        Select();
      } else if (op == "modify") {
        Modify();
      } else if (op == "import") {
        Import();
      } else if (op == "quit" || op == "exit") {
        Exit();
        break;
      } else if (op == "report") {
        Report();
      } else if (op == "log") {
        ShowLog();
      }  else if (op == "") {
        // DO NOTHING!
      } else {
        puts("Invalid");
      }
    } catch (Invalid res) {
      std::cout << res.what() << '\n';
    }
  }
  return 0;
}

/*
7:
report employee
log
*/

void init() {
  account_data.initialise();
  int index;
  account_data.get_info(index, 1);
  if (index == 0) {
    Account rt("root", "sjtu", "root", 7);
    index = account_data.write(rt);
    account_data.write_info(index, 1);
    account_block.insert(((std::string) "root").data(), index);
  }

  book_data.initialise();

  finance_data.initialise();
  finance_data.get_info(index, 1);
  if (index == 0) {
    Finance tmp;
    index = finance_data.write(tmp);
    finance_data.write_info(index, 1);
    finance_data.write_info(0, 2);
  }

  log_data.initialise();
  log_data.get_info(index, 1);
  if (index == 0) {
    Log tmp;
    index = log_data.write(tmp);
    log_data.write_info(index, 1);
    log_data.write_info(index, 2);
  }
}
void Exit() {
  if (command.getstr() != "") {
    throw Invalid();
  }
}
void Su() {
  std::string userID = command.getstr();
  std::string password = command.getstr();
  if (!CheckUserID(userID)) {
    throw Invalid();
  } else if (command.getstr() != "") {
    throw Invalid();
  }
  std::vector<int> pos = account_block.find(userID.data());
  if (pos.size() == 0) {
    throw Invalid();
  }
  int index = pos[0];
  Account user;
  account_data.read(user, index);
  if (password != "" && !CheckPassward(password)) {
    throw Invalid();
  } else if (password == "" && now_user.privilege <= user.privilege) {
    throw Invalid();
  }
  if (password == "" && now_user.privilege > user.privilege) {
    login_stack.push_back(user);
    now_user = user;
    login_state[user.userID]++;
  } else if (password != "" && strcmp(password.data(), user.password) == 0) {
    login_stack.push_back(user);
    now_user = user;
    login_state[user.userID]++;
  } else {
    throw Invalid();
  }

  Log log, las_log;
  int las_index, new_index;
  strncpy(log.op ,((std::string)"Su").data(), log.MAX_LEN);
  log.user = now_user;
  log_data.get_info(las_index, 2);
  log_data.read(las_log, las_index);
  new_index = log_data.write(log);
  las_log.nxt = new_index;
  log_data.update(las_log, las_index);
  log_data.write_info(new_index ,2);
}
void Logout() {
  if (command.getstr() != "") {
    throw Invalid();
  }
  if (login_stack.empty()) {
    throw Invalid();
  }
  Account user = login_stack.back();
  login_state[user.userID]--;
  login_stack.pop_back();
  if (login_stack.empty()) {
    now_user = Account();
  } else {
    now_user = login_stack.back();
  }

  Log log, las_log;
  int las_index, new_index;
  strncpy(log.op ,((std::string)"Logout").data(), log.MAX_LEN);
  log.user = user;
  log_data.get_info(las_index, 2);
  log_data.read(las_log, las_index);
  new_index = log_data.write(log);
  las_log.nxt = new_index;
  log_data.update(las_log, las_index);
  log_data.write_info(new_index ,2);
}
void Register() {
  std::string userID = command.getstr();
  std::string password = command.getstr();
  std::string username = command.getstr();
  if (!CheckUserID(userID) || !CheckPassward(password) ||
      !CheckUsername(username)) {
    throw Invalid();
  } else if (command.getstr() != "") {
    throw Invalid();
  }
  std::vector<int> pos = account_block.find(userID.data());
  if (pos.size() != 0) {
    throw Invalid();
  }
  Account user(userID.data(), password.data(), username.data());
  int index = account_data.write(user);
  account_block.insert(userID.data(), index);

  Log log, las_log;
  int las_index, new_index;
  strncpy(log.op ,((std::string)"Register").data(), log.MAX_LEN);
  log.user = now_user;
  log.target = user;
  log_data.get_info(las_index, 2);
  log_data.read(las_log, las_index);
  new_index = log_data.write(log);
  las_log.nxt = new_index;
  log_data.update(las_log, las_index);
  log_data.write_info(new_index ,2);
}
void Passwd() {
  if (login_stack.empty()) {
    throw Invalid();
  }
  std::string userID = command.getstr();
  std::string now_password = command.getstr();
  std::string new_password = command.getstr();
  if (!CheckUserID(userID)) {
    throw Invalid();
  }
  std::vector<int> pos = account_block.find(userID.data());
  if (command.getstr() != "") {
    throw Invalid();
  } else if (pos.size() == 0) {
    throw Invalid();
  }
  int index = pos[0];
  Account user;
  account_data.read(user, index);
  if (now_user.privilege == 7 && new_password == "") {
    new_password = now_password;
    now_password = user.password;
    if (!CheckPassward(new_password)) {
      throw Invalid();
    }
  } else if (!CheckPassward(new_password) || !CheckPassward(now_password)) {
    throw Invalid();
  }
  if (now_password != user.password) {
    throw Invalid();
  }
  user.passwd(new_password.data());
  account_data.update(user, index);

  Log log, las_log;
  int las_index, new_index;
  strncpy(log.op ,((std::string)"Passwd").data(), log.MAX_LEN);
  log.user = now_user;
  log.target = user;
  strncpy(log.password, new_password.data(), log.MAX_LEN);
  log_data.get_info(las_index, 2);
  log_data.read(las_log, las_index);
  new_index = log_data.write(log);
  las_log.nxt = new_index;
  log_data.update(las_log, las_index);
  log_data.write_info(new_index ,2);
}
void Useradd() {
  if (login_stack.empty() || now_user.privilege < 3) {
    throw Invalid();
  }
  std::string userID = command.getstr();
  std::string password = command.getstr();
  std::string privilege_str = command.getstr();
  std::string username = command.getstr();
  if (!CheckPrivilege(privilege_str) || !CheckPassward(password) ||
      !CheckUserID(userID) || !CheckUsername(username)) {
    throw Invalid();
  }
  int privilege = privilege_str[0] - '0';
  std::vector<int> pos = account_block.find(userID.data());
  if (command.getstr() != "") {
    throw Invalid();
  } else if (privilege >= now_user.privilege) {
    throw Invalid();
  } else if (pos.size() != 0) {
    throw Invalid();
  }
  Account user(userID.data(), password.data(), username.data(), privilege);
  int index = account_data.write(user);
  account_block.insert(userID.data(), index);

  Log log, las_log;
  int las_index, new_index;
  strncpy(log.op ,((std::string)"Useradd").data(), log.MAX_LEN);
  log.user = now_user;
  log.target = user;
  log_data.get_info(las_index, 2);
  log_data.read(las_log, las_index);
  new_index = log_data.write(log);
  las_log.nxt = new_index;
  log_data.update(las_log, las_index);
  log_data.write_info(new_index ,2);
}
void Delete() {
  if (login_stack.empty() || now_user.privilege < 7) {
    throw Invalid();
  }
  std::string userID = command.getstr();
  if (!CheckUserID(userID)) {
    throw Invalid();
  } else if (command.getstr() != "") {
    throw Invalid();
  }
  std::vector<int> pos = account_block.find(userID.data());
  if (pos.size() == 0) {
    throw Invalid();
  }
  int index = pos[0];
  Account user;
  account_data.read(user, index);
  if (login_state[user.userID] != 0) {
    throw Invalid();
  }
  user.Delete();
  account_block.del(userID.data(), index);
  account_data.Delete(index);

  Log log, las_log;
  int las_index, new_index;
  strncpy(log.op ,((std::string)"Delete").data(), log.MAX_LEN);
  log.user = now_user;
  log.target = user;
  log_data.get_info(las_index, 2);
  log_data.read(las_log, las_index);
  new_index = log_data.write(log);
  las_log.nxt = new_index;
  log_data.update(las_log, las_index);
  log_data.write_info(new_index ,2);
}
void Buy() {
  if (login_stack.empty()) {
    throw Invalid();
  }
  std::string ISBN = command.getstr();
  std::string quantity_str = command.getstr();
  if (!CheckISBN(ISBN) || !CheckQuantity(quantity_str)) {
    throw Invalid();
  } else if (command.getstr() != "") {
    throw Invalid();
  }
  int quantity = StringToInt(quantity_str);
  std::vector<int> pos = book_ISBN_block.find(ISBN.data());
  if (quantity <= 0) {
    throw Invalid();
  } else if (pos.empty()) {
    throw Invalid();
  }
  int index = pos[0];
  Book book;
  book_data.read(book, index);
  if (book.stock < quantity) {
    throw Invalid();
  }
  double cost = book.price * quantity;
  printf("%.2lf\n", cost);
  book.stock -= quantity;
  book_data.update(book, index);

  int finance_index;
  finance_data.get_info(finance_index, 1);
  Finance tmp(cost, 0 , book.ISBN, now_user.userID, finance_index);
  // = (Finance){cost, 0, finance_index};
  finance_index = finance_data.write(tmp);
  finance_data.write_info(finance_index, 1);
  int count;
  finance_data.get_info(count, 2);
  count++;
  finance_data.write_info(count, 2);

  Log log, las_log;
  int las_index, new_index;
  strncpy(log.op ,((std::string)"Buy").data(), log.MAX_LEN);
  log.user = now_user;
  log.book = book;
  log.quantity = quantity;
  log.cost = cost;
  log_data.get_info(las_index, 2);
  log_data.read(las_log, las_index);
  new_index = log_data.write(log);
  las_log.nxt = new_index;
  log_data.update(las_log, las_index);
  log_data.write_info(new_index ,2);
}
void Select() {
  if (login_stack.empty() || now_user.privilege < 3) {
    throw Invalid();
  }
  std::string ISBN = command.getstr();
  if (!CheckISBN(ISBN)) {
    throw Invalid();
  } else if (command.getstr() != "") {
    throw Invalid();
  }
  std::vector<int> pos = book_ISBN_block.find(ISBN.data());
  if (pos.empty()) {
    Book book(ISBN.data());
    int index = book_data.write(book);
    book_ISBN_block.insert(ISBN.data(), index);
    now_user.selected_index = index;
    login_stack.back() = now_user;
  } else {
    int index = pos[0];
    now_user.selected_index = index;
    login_stack.back() = now_user;
  }
}
void Import() {
  if (login_stack.empty() || now_user.privilege < 3) {
    throw Invalid();
  } else if (now_user.selected_index == -1) {
    throw Invalid();
  }
  std::string quantity_str = command.getstr();
  std::string cost_str = command.getstr();
  if (!CheckQuantity(quantity_str) || !CheckCost(cost_str)) {
    throw Invalid();
  } else if (command.getstr() != "") {
    throw Invalid();
  }
  int quantity = StringToInt(quantity_str);
  double cost = StringToDouble(cost_str);
  if (quantity <= 0 || cost <= 0) {
    throw Invalid();
  }
  int index = now_user.selected_index;
  Book book;
  book_data.read(book, index);
  book.stock += quantity;
  book_data.update(book, index);

  int finance_index;
  finance_data.get_info(finance_index, 1);
  Finance tmp(0 ,cost , book.ISBN, now_user.userID, finance_index);
  finance_index = finance_data.write(tmp);
  finance_data.write_info(finance_index, 1);
  int count;
  finance_data.get_info(count, 2);
  count++;
  finance_data.write_info(count, 2);

  Log log, las_log;
  int las_index, new_index;
  strncpy(log.op ,((std::string)"Import").data(), log.MAX_LEN);
  log.user = now_user;
  log.book = book;
  log.quantity = quantity;
  log.cost = cost;
  log_data.get_info(las_index, 2);
  log_data.read(las_log, las_index);
  new_index = log_data.write(log);
  las_log.nxt = new_index;
  log_data.update(las_log, las_index);
  log_data.write_info(new_index ,2);
}
void CheckParameter(std::string &str, int &type) {
  if (str[0] != '-') {
    type = -1;
    return;
  }
  int pos = -1, len = str.size();
  std::string op = "";
  for (int i = 1; i < len; i++) {
    if (str[i] == '=') {
      pos = i;
      break;
    } else {
      op += str[i];
    }
  }
  if (pos == -1 || pos == len - 1) {
    type = -1;
    return;
  }
  if (op == "ISBN") {
    type = 1;
  } else if (op == "name") {
    type = 2;
  } else if (op == "author") {
    type = 3;
  } else if (op == "keyword") {
    type = 4;
  } else if (op == "price") {
    type = 5;
  } else {
    type = -1;
    return;
  }
  int l = pos + 1, r = len - 1;
  if (type >= 2 && type <= 4) {
    if (str[l] != '\"' || str[r] != '\"') {
      type = -1;
      return;
    } else {
      l++;
      r--;
    }
  }
  std::string para = "";
  for (int i = l; i <= r; i++) {
    para += str[i];
  }
  if (para == "") {
    type = -1;
    return;
  }
  str = para;
}
void UpdateKeyBlock(const std::string &str, int index, int op) {
  int sz = str.size();
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
    if (op == 1) {
      book_key_block.insert(key[i].data(), index);
    } else {
      book_key_block.del(key[i].data(), index);
    }
  }
}
void GetBookInformation(std::string &ISBN, std::string &name,
                        std::string &author, std::string &key,
                        std::string &price_str, int op) {
  // op = 1 : Modify
  // op = 2 : Show
  std::string tmp = command.getstr();
  std::vector<std::string> parameter;
  while (tmp != "") {
    parameter.push_back(tmp);
    tmp = command.getstr();
  }
  if (op == 1 && parameter.empty()) {
    throw Invalid();
  }
  int last_type = -1;
  for (std::string str : parameter) {
    int type;
    CheckParameter(str, type);
    if (type <= 0 || type >= 6) {
      throw Invalid();
    } else if (type == 1) {
      (ISBN != "") ? (throw Invalid()) : (ISBN = str);
    } else if (type == 2) {
      (name != "") ? (throw Invalid()) : (name = str);
    } else if (type == 3) {
      (author != "") ? (throw Invalid()) : (author = str);
    } else if (type == 4) {
      (key != "") ? (throw Invalid()) : (key = str);
    } else if (type == 5 && op == 1) {
      (price_str != "") ? (throw Invalid()) : (price_str = str);
    } else {
      throw Invalid();
    }
    if (op == 2 && last_type != -1) {
      throw Invalid();
    }
    last_type = type;
  }
}
void Modify() {
  if (login_stack.empty() || now_user.privilege < 3) {
    throw Invalid();
  } else if (now_user.selected_index == -1) {
    throw Invalid();
  }
  std::string ISBN = "", name = "", author = "", key = "", price_str = "";
  GetBookInformation(ISBN, name, author, key, price_str, 1);
  if (ISBN != "" && !CheckISBN(ISBN)) {
    throw Invalid();
  } else if (name != "" && !CheckBookName(name)) {
    throw Invalid();
  } else if (author != "" && !CheckAuthor(author)) {
    throw Invalid();
  } else if (key != "" && !CheckKeyword(key)) {
    throw Invalid();
  } else if (price_str != "" && !CheckPrice(price_str)) {
    throw Invalid();
  }
  if (ISBN != "" && book_ISBN_block.find(ISBN.data()).size()) {
    throw Invalid();
  }
  int index = now_user.selected_index;
  Book book, old_book;
  book_data.read(book, index);
  old_book = book;
  if (book.ISBN[0] != '\0' && ISBN != "") {
    book_ISBN_block.del(book.ISBN, index);
  }
  if (book.book_name[0] != '\0' && name != "") {
    book_name_block.del(book.book_name, index);
  }
  if (book.auther[0] != '\0' && author != "") {
    book_author_block.del(book.auther, index);
  }
  if (book.keyword[0] != '\0' && key != "") {
    UpdateKeyBlock(book.keyword, index, -1);
  }
  if (ISBN != "") {
    strncpy(book.ISBN, ISBN.data(), book.ISBN_LEN);
    book_ISBN_block.insert(book.ISBN, index);
  }
  if (name != "") {
    strncpy(book.book_name, name.data(), book.MAX_LEN);
    book_name_block.insert(book.book_name, index);
  }
  if (author != "") {
    strncpy(book.auther, author.data(), book.MAX_LEN);
    book_author_block.insert(book.auther, index);
  }
  if (key != "") {
    strncpy(book.keyword, key.data(), book.MAX_LEN);
    UpdateKeyBlock(book.keyword, index, 1);
  }
  if (price_str != "") {
    double price = StringToDouble(price_str);
    book.price = price;
  }
  book_data.update(book, index);

  Log log, las_log;
  int las_index, new_index;
  strncpy(log.op ,((std::string)"Modify").data(), log.MAX_LEN);
  log.user = now_user;
  log.old_book = old_book;
  log.book = book;
  log_data.get_info(las_index, 2);
  log_data.read(las_log, las_index);
  new_index = log_data.write(log);
  las_log.nxt = new_index;
  log_data.update(las_log, las_index);
  log_data.write_info(new_index ,2);
}
int KeywordNumber(const std::string &str) {
  int cnt = 1, sz = str.size();
  for (int i = 0; i < sz; i++) {
    if (str[i] == '|') {
      cnt++;
    }
  }
  return cnt;
}
bool FindKeyword(const Book &book, const std::string &key) {
  std::string str = book.keyword;
  int sz = str.size();
  std::string tmp = "";
  for (int i = 0; i < sz; i++) {
    if (str[i] == '|' && tmp == key) {
      return true;
    } else if (str[i] == '|') {
      tmp = "";
    } else {
      tmp += str[i];
    }
  }
  return (tmp == key);
}
void ShowBook() {
  if (login_stack.empty()) {
    throw Invalid();
  }
  std::string ISBN = "", name = "", author = "", key = "", price_str = "";
  GetBookInformation(ISBN, name, author, key, price_str, 2);
  if (ISBN != "" && !CheckISBN(ISBN)) {
    throw Invalid();
  } else if (name != "" && !CheckBookName(name)) {
    throw Invalid();
  } else if (author != "" && !CheckAuthor(author)) {
    throw Invalid();
  } else if (key != "" && !CheckKeyword(key)) {
    throw Invalid();
  } else if (price_str != "" && !CheckPrice(price_str)) {
    throw Invalid();
  } else if (key != "" && KeywordNumber(key) > 1) {
    throw Invalid();
  }
  std::vector<int> pos;
  if (ISBN != "") {
    pos = book_ISBN_block.find(ISBN.data());
  } else if (name != "") {
    pos = book_name_block.find(name.data());
  } else if (author != "") {
    pos = book_author_block.find(author.data());
  } else if (key != "") {
    pos = book_key_block.find(key.data());
  } else {
    pos = book_ISBN_block.findall();
  }
  std::vector<Book> ans;
  for (int index : pos) {
    Book book;
    book_data.read(book, index);
    ans.push_back(book);
  }
  std::sort(ans.begin(), ans.end(), cmpISBN);
  for (Book book : ans) {
    std::cout << book.ISBN << '\t' << book.book_name << '\t' << book.auther
              << '\t' << book.keyword;
    printf("\t%.2lf\t", book.price);
    std::cout << book.stock << '\n';
  }
  if (ans.empty()) {
    puts("");
  }
}
void ShowFinance() {
  if (login_stack.empty() || now_user.privilege < 7) {
    throw Invalid();
  }
  std::string count_str = command.getstr();
  if (count_str != "" && !CheckCount(count_str)) {
    throw Invalid();
  } else if (command.getstr() != "") {
    throw Invalid();
  }
  int total_count, count;
  double income = 0, expenditure = 0;
  finance_data.get_info(total_count, 2);
  if (count_str == "") {
    count = total_count;
  } else {
    count = StringToInt(count_str);
  }
  if (count > total_count) {
    throw Invalid();
  }
  if (count_str != "" && count == 0) {
    puts("");
    return;
  }
  int index;
  finance_data.get_info(index, 1);
  Finance tmp;
  while (count--) {
    finance_data.read(tmp, index);
    income += tmp.income;
    expenditure += tmp.expenditure;
    index = tmp.pre;
  }
  printf("+ %.2lf - %.2lf\n", income, expenditure);
}
void Show() {
  std::string tmp = command.getstr();
  if (tmp == "") {
    ShowBook();
  } else if (tmp == "finance") {
    ShowFinance();
  } else {
    command.moveback();
    ShowBook();
  }
}