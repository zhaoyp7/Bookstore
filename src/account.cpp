#include "account.hpp"

Account::Account() {
  userID[0] = '\0';
  password[0] = '\0';
  username[0] = '\0';
  selected_index = -1;
  privilege = 0;
}
Account::Account(const char *id, const char *pw, const char *name, int type) {
  strncpy(userID, id, MAX_LEN);
  strncpy(password, pw, MAX_LEN);
  strncpy(username, name, MAX_LEN);
  selected_index = -1;
  privilege = type;
}
void Account::passwd(char *new_pw) { strncpy(password, new_pw, MAX_LEN); }
void Account::Delete() {}