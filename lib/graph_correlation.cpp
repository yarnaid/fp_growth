#include "graph_correlation.h"

#include <vector>
#include <iostream>
#include <queue>

struct Edge;
using item_ptr = std::shared_ptr<const Item>;
using node_ptr = std::shared_ptr<FPNode>;

struct Edge
{
    node_ptr source;
    node_ptr target;
    inline bool operator <(const Edge& other) const
    {
        return this->source->token < other.source->token;
    }
    inline bool operator ==(const Edge& other) const
    {
        return (this->source->token == other.source->token) ||
                (this->target->token == other.target->token);
    }
};


void fill_nodes(std::map<url_t, node_ptr>& nodes_by_url,
           std::map<ip_t, node_ptr>& nodes_by_ip,
           const FPTree& fptree);


void update_urls(std::set<url_t>& urls,
                 std::set<ip_t>& ips,
                 std::set<Edge>& edges,
                 std::map<ip_t, node_ptr>& nodes_by_ip,
                 std::map<url_t, node_ptr>& nodes_by_url)
{
    for (auto url : urls)
    {
        std::set<ip_t> new_ips;
        for (auto url : urls)
            new_ips.insert(nodes_by_url[url]->item.ip);
        for (auto ip : new_ips)
        {
            ips.insert(ip);
            Edge e {nodes_by_url[url], nodes_by_ip[ip]};
            edges.insert(e);
        }
    }
}


void update_ips(std::set<url_t>& urls,
                 std::set<ip_t>& ips,
                 std::set<Edge>& edges,
                 std::map<ip_t, node_ptr>& nodes_by_ip,
                 std::map<url_t, node_ptr>& nodes_by_url)
{
    for (auto ip : ips)
    {
        std::set<url_t> new_urls;
        for (auto ip : ips)
            new_urls.insert(nodes_by_ip[ip]->item.url);
        for (auto url : new_urls)
        {
            urls.insert(url);
            Edge e {nodes_by_url[url], nodes_by_ip[ip]};
            edges.insert(e);
        }
    }
}


void fill_urls(std::set<url_t>& urls, const std::map<std::string, URLStat>& stats);


void build_graph(const std::map<std::string, URLStat>& stats,
                 const FPTree& fptree)
{
    std::set<url_t> urls;
    std::set<ip_t> ips;

    std::map<url_t, node_ptr> nodes_by_url;
    std::map<ip_t, node_ptr> nodes_by_ip;

    fill_nodes(nodes_by_url, nodes_by_url, fptree);
    fill_urls(urls, stats);

    std::set<Edge> edges;

    int edges_num = edges.size();
    int old_edges_num = 0;
    if (urls.size() > 0)
    {
        old_edges_num = -1;
    }


    while (edges_num > old_edges_num)
    {
        old_edges_num = edges_num;
        update_urls(urls, ips, edges, nodes_by_ip, nodes_by_url);
//        update_ips(urls, ips, edges, nodes_by_ip, nodes_by_url);
        edges_num = edges.size();
        std::cout << edges_num << std::endl;
    }
}


void fill_nodes(std::map<url_t, node_ptr>& nodes_by_url,
           std::map<ip_t, node_ptr>& nodes_by_ip,
           const FPTree& fptree)
{
    std::queue<node_ptr> q;
    q.push(fptree.root);
    while(!q.empty())
    {
        node_ptr node = q.front();
        q.pop();

        for(node_ptr child : node->children)
            q.push(child);
        nodes_by_ip[node->item.ip] = node;
        nodes_by_url[node->item.fqdn] = node;
    }
    return;
}



void fill_urls(std::set<url_t>& urls, const std::map<std::string, URLStat>& stats)
{
    for (auto stat : stats)
    {
        urls.insert(stat.first);
    }
    return;
}
