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
#include "../models/transaction.h"
#include "../models/pattern.h"

struct FPTree {
    std::shared_ptr<FPNode> root;
    std::map<Item, std::shared_ptr<FPNode>> header_table;
    double minimum_support_treshold;
    double maximum_support_treshold;

    FPTree(const std::vector<Transaction>&, double, double );

    bool empty() const;
};


std::set<Pattern> fptree_growth(const FPTree&, const unsigned& class_treshold);
void print_pattern(const std::set<Pattern>&);
void dump_pattern(const std::set<Pattern>& patterns, const std::string& file_name);



#endif  // FPTREE_HPP
