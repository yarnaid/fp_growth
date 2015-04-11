#ifndef FPNODE_H
#define FPNODE_H

#include <vector>
#include <string>
#include "item.h"



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

#endif // FPNODE_H
