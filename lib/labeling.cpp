#include "labeling.h"

#include <vector>

void labeling(FPTree& fptree) {
    if (fptree.empty()) {
        return;
    }
    std::vector<std::shared_ptr<FPNode>> centers = fptree.root->children;

    // don't know what it for, but can be usefull
    for (int i = 0; i < centers.size(); ++i) {
        centers[i]->label = i;
    }

    for (auto center : centers) {
        const Item item = center->item;
        std::cout << item.fqdn << " " << center->frequency << std::endl;
    }
    return;
}
