#ifndef FPNODE_H
#define FPNODE_H

#include <vector>
#include <string>
#include "item.h"
#include <memory>



struct FPNode
{
    const Item item;
    unsigned frequency;
    std::shared_ptr<FPNode> node_link;
    std::shared_ptr<FPNode> parent;
    std::vector<std::shared_ptr<FPNode>> children;
    std::string token;
    unsigned depth;
    unsigned campaign;

    unsigned label;

    bool operator=(const FPNode& other) const {return item == other.item;}
    bool operator<(const FPNode& other) const {return item < other.item;}

    FPNode(const Item&, const std::shared_ptr<FPNode>&);
};

#endif // FPNODE_H
