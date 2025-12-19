#pragma once

#include <cstring>
#include <iostream>
#include <algorithm>
#include <vector>

bool CheckUserID(const std::string &str);
bool CheckPassward(const std::string &str);
bool CheckPrivilege(const std::string &str);
bool CheckUsername(const std::string &str);
bool CheckISBN(const std::string &str);
bool CheckBookName(const std::string &str);
bool CheckAuthor(const std::string &str);
bool CheckKeyword(const std::string &str);
bool CheckQuantity(const std::string &str);
bool CheckPrice(const std::string &str);
bool CheckCost(const std::string &str);
bool CheckCount(const std::string &str);
double StringToDouble(const std::string &str);
int StringToInt(const std::string &str);