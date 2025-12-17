#include "BlockLinkedList.cpp"
#include "MemoryRiver.hpp"
#include "account.cpp"
#include "command.cpp"
#include <iostream>
#include <map>
#include <vector>

std::vector<Account> login_stack;
MemoryRiver<Account> account_data("account_data");
/*
info:
1: root's index
2:
*/
BlockLinkedList account_block("account_block");
Command command;
Account now_user;
std::map<std::string, int> login_state;

class Invalid {
private:
  const char *message;

public:
  Invalid() : message("Invalid"){};
  const char *what() const { return message; }
};

bool CheckUserID(std::string &str) {
  int sz = str.size();
  if (sz > 30 || sz == 0) {
    return false;
  }
  for (int i = 0; i < sz; i++) {
    bool flag = 0;
    flag |= (str[i] >= '0'&&str[i] <= '9');
    flag |= (str[i] >= 'a'&&str[i] <= 'z');
    flag |= (str[i] >= 'A'&&str[i] <= 'Z');
    flag |= (str[i] == '_');
    if (!flag) {
      return false;
    }
  }
  return true;
}
bool CheckPassward(std::string &str) {
  return CheckUserID(str);
}
bool CheckPrivilege(std::string &str) {
  int sz = str.size();
  if (sz > 1 || sz == 0) {
    return false;
  }
  return (str[0] >= '0'&&str[0] <= '9');
}
bool CheckUsername(std::string &str) {
  int sz = str.size();
  if (sz == 0) {
    return false;
  }
  return true;
}

void init() {
  account_data.initialise();
  int index;
  account_data.get_info(index, 1);
  // printf("root index = %d\n",index);
  if (index == 0) {
    // puts("build root account");
    Account rt("root", "sjtu", "root", 7);
    index = account_data.write(rt);
    // printf("index = %d\n",index);
    account_data.write_info(index, 1);
    account_block.insert(((std::string) "root").data(), index);
  }
}
void Su() {
  std::string userID = command.getstr();
  std::string password = command.getstr();
  if (!CheckUserID(userID) || !CheckPassward(password)) {
    throw Invalid();
  }
  // std::cout << userID << ' ' << password << '\n';
  std::vector<int> pos = account_block.find(userID.data());
  if (command.getstr() != "") {
    throw Invalid();
  }
  if (pos.size() == 0) {
    throw Invalid();
  }
  int index = pos[0];
  // printf("index = %d\n",index);
  Account user;
  account_data.read(user, index);
  // return ; 
  // std::cout << user.userID << ' ' << user.password << ' ' << user.privilege << ' ' << user.username << '\n';
  // return ; 

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
}
void Register() {
  std::string userID = command.getstr();
  std::string password = command.getstr();
  std::string username = command.getstr();
  std::vector<int> pos = account_block.find(userID.data());
  if (!CheckUserID(userID) || !CheckPassward(password) || !CheckUsername(username)) {
    throw Invalid();
  }
  if (command.getstr() != "") {
    throw Invalid();
  }
  if (pos.size() != 0) {
    throw Invalid();
  }
  Account user(userID, password, username);
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
  std::vector<int> pos = account_block.find(userID.data());
  if (command.getstr() != "") {
    throw Invalid();
  }
  if (pos.size() == 0) {
    throw Invalid();
  }
  int index = pos[0];
  Account user;
  account_data.read(user, index);
  if (now_user.privilege == 7 && new_password == "") {
    new_password = now_password;
    now_password = user.password;
    if (!CheckUserID(userID) || !CheckPassward(new_password)) {
      throw Invalid();
    }
  } else if (!CheckUserID(userID) || !CheckPassward(new_password) || !CheckPassward(now_password)) {
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
  if (!CheckPrivilege(privilege_str) || !CheckPassward(password) || !CheckUserID(userID) || !CheckUsername(username)) {
    throw Invalid();
  }
  // std::cout << userID << ' ' << password << ' ' << privilege << ' ' << username << '\n';
  int privilege = privilege_str[0] - '0';
  if (command.getstr() != "") {
    throw Invalid();
  }
  if (privilege >= now_user.privilege) {
    throw Invalid();
  }
  std::vector<int> pos = account_block.find(userID.data());
  if (pos.size() != 0) {
    throw Invalid();
  }
  Account user(userID, password, username, privilege);
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
  }
  std::vector<int> pos = account_block.find(userID.data());
  if (command.getstr() != "") {
    throw Invalid();
  }
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
      } else if (op == "quit" || op == "exit") {
        break;;
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
0:
su
register
1:
logout
passwd
3:
useradd
7:
delete

1:
show
buy
3:
select
modify
import
7:
show

7:
report finance
report employee
log
*/