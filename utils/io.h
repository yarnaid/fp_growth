#include <iostream>

#include "../FP-growth/fptree.hpp"



int read_transactions(const std::string& filename,
    std::vector<Transaction>& transactions,
    std::vector<Item>& items);