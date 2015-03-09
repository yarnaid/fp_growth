#include <iostream>

#include "../FP-growth/fptree.hpp"



int read_transactions(const std::string& filename,
    std::vector<Transaction>& transactions,
    std::vector<Item>& items);

std::string operator"" _quoted(const char* text, std::size_t len);
std::string quote(const std::string& s);
std::string quote(const double& s);
