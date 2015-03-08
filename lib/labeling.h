#ifndef LABELING_H
#define LABELING_H

#include <map>
#include <string>
#include <vector>
#include "../utils/utils.h"

#include "../FP-growth/fptree.hpp"

void labeling(FPTree& fptree);

using stat_map = std::map<std::string, double>;
using node_ptr = std::shared_ptr<FPNode>;

struct StatData
{
    node_ptr node;
    stat_map stat;
};

#endif // LABELING_H
