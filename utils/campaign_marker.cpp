#include "campaign_marker.h"

#include <vector>
#include <queue>
#include <fstream>
#include "../models/fpnode.h"


using node_ptr = std::shared_ptr<FPNode>;

std::string ext = ".csv";
std::string file_prefix = "campaign_items_";

void mark_tree(const FPTree &tree, const bool& dump)
{
    std::vector<node_ptr> &campaigns = tree.root->children;

    unsigned k = 0;
    for (node_ptr root : campaigns)
    {
        std::ofstream out_file;
        if (dump == true)
            out_file.open(file_prefix + std::to_string(k) + ext);
        // mark all children with k
        std::queue<node_ptr> q;
        q.push(root);
        while (!q.empty())
        {
            node_ptr node = q.front();
            q.pop();
            for (auto child : node->children)
                q.push(child);
            node->campaign = k;
            if (out_file.is_open())
            {
                out_file << "\"" << k << "\"," << node->item << std::endl;
            }
        }
        if (out_file.is_open())
            out_file.close();

        // go to new campaign
        ++k;
    }
}
