#ifndef IO_H
#define IO_H

#include <iostream>

#include <string>
#include <vector>

#include "../models/transaction.h"
#include "../models/item.h"
#include "../models/url.h"


int read_transactions(const std::string& filename,
                      std::vector<Transaction>& transactions,
                      std::vector<Item>& items);

std::vector<std::string> url_to_tokens(const std::string& url);

std::string operator"" _quoted(const char* text, std::size_t len);
std::string quote(const std::vector<int>& s);
std::string quote(const std::string& s);
std::string quote(const double& s);

#endif // IO_H
