#include "log.hpp"

Finance::Finance() {
  income = 0;
  expenditure = 0;
  ISBN[0] = '\0';
  userID[0] = '\0';
  pre = 0;
}
Finance::Finance(double in, double out, char *isbn, char *id, int pr) {
  income = in;
  expenditure = out;
  strncpy(ISBN, isbn, ISBN_LEN);
  strncpy(userID, id, MAX_LEN);
  pre = pr;
}

Log::Log() {
  op[0] == '\0';
  password[0] = '\0';
  quantity = 0;
  cost = 0;
  nxt = 0;
}