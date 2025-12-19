#pragma once

#include <cstring>
#include <iostream>

class Invalid {
private:
  const char *message;

public:
  Invalid() : message("Invalid"){};
  const char *what() const { return message; }
};

class Command {
private:
  int sz, pos;
  std::string str;

public:
  bool read();
  std::string getstr();
  void moveback();
};