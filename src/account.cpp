#include "account.hpp"

Account::Account() : privilege(0), selected_index(-1) {
  userID[0] = '\0';
  password[0] = '\0';
  username[0] = '\0';
}
Account::Account(const char *id, const char *pw, const char *name, int type)
    : privilege(type), selected_index(-1) {
  strncpy(userID, id, MAX_LEN);
  strncpy(password, pw, MAX_LEN);
  strncpy(username, name, MAX_LEN);
}
void Account::passwd(char *new_pw) { strncpy(password, new_pw, MAX_LEN); }
void Account::Delete() {}