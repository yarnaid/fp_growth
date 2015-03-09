#include "graph_correlation.h"

#include <vector>
#include <iostream>
#include <queue>

struct Edge;
using item_ptr = std::shared_ptr<const Item>;
using node_ptr = std::shared_ptr<FPNode>;

void update_urls() {}
void update_ips() {}

void fill_urls(std::set<std::string>& urls, const std::map<std::string, URLStat>& stats);


//void flatten_tree(const FPTree& fptree, std::set<item_ptr>& items)
//{
//    std::queue<node_ptr> q;
//    q.push(fptree.root);
//    while (!q.empty())
//    {
//        node_ptr node = q.front();
//        q.pop();

//        items.push(new item_ptr(&(node->item)));
//        for (node_ptr child : node->children)
//            q.push(child);
//    }
//}

void build_graph(const std::map<std::string, URLStat>& stats,
                 const FPTree& fptree)
{
    std::set<std::string> urls;
    std::set<std::string> ips;

    std::set<item_ptr> items_done;
    std::set<item_ptr> items;

//    flatten_tree(fptree, items);

    fill_urls(urls, stats);

    while ((!urls.empty() && !ips.empty()) || (items_done != items))
    {
        update_urls();
        update_ips();
    }
}

struct Edge
{
    item_ptr source;
    item_ptr target;
};




void fill_urls(std::set<std::string>& urls, const std::map<std::string, URLStat>& stats)
{
    return;
}
