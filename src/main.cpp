#include "BlockLinkedList.cpp"
#include "MemoryRiver.hpp"
#include "account.cpp"
#include "book.cpp"
#include "command.cpp"
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
  double income, expenditure;
  int pre;
};
MemoryRiver<Finance> finance_data("finance_data");
// info1: last index
// info2: Total Count

Command command;
Account now_user;
std::map<std::string, int> login_state;
std::vector<Account> login_stack;

bool cmpISBN(const Book &a, const Book &b) {
  return strcmp(a.ISBN, b.ISBN) < 0;
}

bool CheckUserID(const std::string &str);
bool CheckPassward(const std::string &str) { return CheckUserID(str); }
bool CheckPrivilege(const std::string &str);
bool CheckUsername(const std::string &str);
bool CheckISBN(const std::string &str);
bool CheckBookName(const std::string &str);
bool CheckAuthor(const std::string &str) { return CheckBookName(str); }
bool CheckKeyword(const std::string &str);
bool CheckQuantity(const std::string &str);
bool CheckPrice(const std::string &str);
bool CheckCost(const std::string &str) { return CheckPrice(str); }
bool CheckCount(const std::string &str) { return CheckQuantity(str); }
double StringToDouble(const std::string &str);
int StringToInt(const std::string &str);

void init();
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
        break;
      } else if (op == "") {
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
report finance
report employee
log
*/

bool CheckUserID(const std::string &str) {
  int sz = str.size();
  if (sz > 30 || sz == 0) {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    bool flag = 0;
    flag |= (str[i] >= '0' && str[i] <= '9');
    flag |= (str[i] >= 'a' && str[i] <= 'z');
    flag |= (str[i] >= 'A' && str[i] <= 'Z');
    flag |= (str[i] == '_');
    if (!flag) {
      return false;
    }
  }
  return true;
}
bool CheckPrivilege(const std::string &str) {
  int sz = str.size();
  if (sz > 1 || sz == 0) {
    return false;
  }
  return (str[0] == '1' || str[0] == '3' || str[0] == '7');
}
bool CheckUsername(const std::string &str) {
  int sz = str.size();
  if (sz == 0) {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    if (str[i] < 32 || str[i] > 126) {
      return false;
    }
  }
  return true;
}
bool CheckISBN(const std::string &str) {
  int sz = str.size();
  if (sz > 20 || sz == 0) {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    if (str[i] < 32 || str[i] > 126) {
      return false;
    }
  }
  return true;
}
bool CheckBookName(const std::string &str) {
  int sz = str.size();
  if (sz > 60 || sz == 0) {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    if (str[i] == '\"') {
      return false;
    } else if (str[i] < 32 || str[i] > 126) {
      return false;
    }
  }
  return true;
}
bool CheckKeyword(const std::string &str) {
  int sz = str.size();
  if (sz > 60 || sz == 0 || str[0] == '|' || str[sz - 1] == '|') {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    if (str[i] == '\"') {
      return false;
    } else if (str[i] < 32 || str[i] > 126) {
      return false;
    }
  }
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
    if (key[i] == "") {
      return false;
    }
  }
  for (int i = 0; i + 1 < key_num; i++) {
    if (key[i] == key[i + 1]) {
      return false;
    }
  }
  return true;
}
bool CheckQuantity(const std::string &str) {
  int sz = str.size();
  if (sz > 10 || sz == 0) {
    return false;
  } else if (sz > 1 && str[0] == '0') {
    return false;
  }
  long long sum = 0;
  for (int i = 0; i < sz; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return false;
    }
    sum = sum * 10 + str[i] - '0';
  }
  if (sum > 2147483647) {
    return false;
  }
  return true;
}
bool CheckPrice(const std::string &str) {
  int sz = str.size();
  if (sz > 13 || sz == 0) {
    return false;
  }
  int dot_num = 0, dot_pos = 0;
  for (int i = 0; i < sz; i++) {
    if (str[i] == '.') {
      dot_num++;
      dot_pos = i;
    } else if (str[i] < '0' || str[i] > '9') {
      return false;
    }
  }
  if (dot_num > 1) {
    return false;
  } else if (dot_num == 1 && (dot_pos == 0 || dot_pos == sz - 1)) {
    return false;
  } else if (dot_num == 1 && dot_pos != 1 && str[0] == '0') {
    return false;
  }
  return true;
}

double StringToDouble(const std::string &str) {
  int sz = str.size(), flag = 1, pos = 0;
  double ans = 0, val = 1;
  bool point = 0;
  while (pos < sz) {
    if (str[pos] == '.') {
      point = 1;
    } else if (point == 0) {
      ans = ans * 10 + str[pos] - '0';
    } else if (point == 1) {
      val *= 0.1;
      ans += (str[pos] - '0') * val;
    }
    pos++;
  }
  return ans;
}
int StringToInt(const std::string &str) {
  int ans = 0, sz = str.size();
  for (int i = 0; i < sz; i++) {
    ans = ans * 10 + str[i] - '0';
  }
  return ans;
}

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
    Finance tmp = (Finance){0, 0, 0};
    index = finance_data.write(tmp);
    finance_data.write_info(index, 1);
    finance_data.write_info(1, 2);
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
  Finance tmp = (Finance){cost, 0, finance_index};
  finance_index = finance_data.write(tmp);
  finance_data.write_info(finance_index, 1);
  int count;
  finance_data.get_info(count, 2);
  count++;
  finance_data.write_info(count, 2);
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
  Finance tmp = (Finance){0, cost, finance_index};
  finance_index = finance_data.write(tmp);
  finance_data.write_info(finance_index, 1);
  int count;
  finance_data.get_info(count, 2);
  count++;
  finance_data.write_info(count, 2);
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
  Book book;
  book_data.read(book, index);
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