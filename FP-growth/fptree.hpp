#ifndef FPTREE_HPP
#define FPTREE_HPP

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <ctime>

#include "../models/item.h"
#include "../models/fpnode.h"


using Transaction = std::vector<Item>;
using TransformedPrefixPath = std::pair<std::vector<Item>, unsigned>;
using Pattern = std::pair<std::set<Item>, unsigned>;


struct FPTree {
    std::shared_ptr<FPNode> root;
    std::map<Item, std::shared_ptr<FPNode>> header_table;
    unsigned minimum_support_treshold;

    FPTree(const std::vector<Transaction>&, unsigned);

    bool empty() const;
};


std::set<Pattern> fptree_growth(const FPTree&);
void print_pattern(const std::set<Pattern>&);


#endif  // FPTREE_HPP
