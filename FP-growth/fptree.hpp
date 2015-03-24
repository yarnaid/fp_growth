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


struct Item {
    time_t time;
    std::string id;
    std::string source;
    std::string category;
    std::string name;
    std::string md5;
    std::string ip;
    std::string url;
    std::string fqdn;
    std::string asn;
    std::string cc;
    std::string details;

    int svm_class;
    std::string token;

    bool operator==(const Item& other) const {
        return (token == other.token);
    }

    bool operator<(const Item& other) const {
        return token < other.token;
    }
};
using Transaction = std::vector<Item>;
using TransformedPrefixPath = std::pair<std::vector<Item>, unsigned>;
using Pattern = std::pair<std::set<Item>, unsigned>;


struct FPNode {
    const Item item;
    unsigned frequency;
    std::shared_ptr<FPNode> node_link;
    std::shared_ptr<FPNode> parent;
    std::vector<std::shared_ptr<FPNode>> children;
    std::string token;
    int depth;

    int label;

    FPNode(const Item&, const std::shared_ptr<FPNode>&);
};



struct FPTree {
    std::shared_ptr<FPNode> root;
    std::map<Item, std::shared_ptr<FPNode>> header_table;
    unsigned minimum_support_treshold;

    FPTree(const std::vector<Transaction>&, unsigned);

    bool empty() const;
};


std::set<Pattern> fptree_growth(const FPTree&);
void print_pattern(const std::set<Pattern>&);
std::ostream& operator<<(std::ostream&, const Item&);


#endif  // FPTREE_HPP
