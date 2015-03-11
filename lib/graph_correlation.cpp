#include "graph_correlation.h"

#include <vector>
#include <iostream>
#include <queue>
#include <ostream>
#include <sstream>

struct Edge;
using item_ptr = std::shared_ptr<const Item>;
using node_ptr = std::shared_ptr<FPNode>;
std::ostream& operator<<(std::ostream& os, const Edge& edge);
void to_json(const std::set<Edge>& edges);
void fill_urls(std::set<url_t>& urls, const std::map<std::string, URLStat>& stats);
void fill_nodes(std::map<url_t, std::set<node_ptr> >& nodes_by_url,
                std::map<ip_t, std::set<node_ptr> >& nodes_by_ip,
                const FPTree& fptree);


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
        return (this->source->token == other.source->token) &&
               (this->target->token == other.target->token);
    }
};


void update_urls(std::set<url_t>& urls,
                 std::set<ip_t>& ips,
                 std::set<Edge>& edges,
                 std::map<ip_t, std::set<node_ptr>>& nodes_by_ip,
                 std::map<url_t, std::set<node_ptr>>& nodes_by_url)
{
    for (auto url : urls)
    {
        for (auto node_ip : nodes_by_url[url])
        {
            ip_t ip = node_ip->item.ip;
            ips.insert(ip);
            for (auto var_url : nodes_by_url[url])
            {
                if (var_url->item.fqdn != node_ip->item.fqdn)
                {
                    Edge e {var_url, node_ip};
                    edges.insert(e);
                }
            }
        }
    }
}


void update_ips(std::set<url_t>& urls,
                std::set<ip_t>& ips,
                std::set<Edge>& edges,
                std::map<ip_t, std::set<node_ptr>>& nodes_by_ip,
                std::map<url_t, std::set<node_ptr>>& nodes_by_url)
{
    for (auto ip : ips)
    {
        for (auto node_url : nodes_by_ip[ip])
        {
            ip_t url = node_url->item.fqdn;
            urls.insert(url);
            for (auto var_ip : nodes_by_ip[ip])
            {
                if (var_ip->item.fqdn != node_url->item.fqdn)
                {
                    Edge e {node_url, var_ip};
                    edges.insert(e);
                }
            }
        }
    }
}


void build_graph(const std::map<std::string, URLStat>& stats,
                 const FPTree& fptree)
{
    std::set<url_t> urls;
    std::set<ip_t> ips;

    std::map<url_t, std::set<node_ptr>> nodes_by_url;
    std::map<ip_t, std::set<node_ptr>> nodes_by_ip;

    fill_nodes(nodes_by_url, nodes_by_ip, fptree);
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
        update_ips(urls, ips, edges, nodes_by_ip, nodes_by_url);
        edges_num = edges.size();
    }

    to_json(edges);
}


void fill_nodes(std::map<url_t, std::set<node_ptr>>& nodes_by_url,
                std::map<ip_t, std::set<node_ptr>>& nodes_by_ip,
                const FPTree& fptree)
{
    std::queue<node_ptr> q;
    q.push(fptree.root);
    while (!q.empty())
    {
        node_ptr node = q.front();
        q.pop();

        for (node_ptr child : node->children)
            q.push(child);

        if (node->item.fqdn != "")
        {
            nodes_by_ip[node->item.ip].insert(node);
            nodes_by_url[node->item.fqdn].insert(node);
        }
    }
    return;
}

std::ostream& operator<<(std::ostream& os, const Edge& edge)
{
    os << "{";
    if (edge.source)
        os << "\"source\":\"" << edge.source->item.fqdn << "\"";
    if (edge.source && edge.target)
        os << ",";
    if (edge.target)
        os << "\"target\":\"" << edge.target->item.fqdn << "\"";
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const node_ptr& node)
{
    os << "{\"url\":\""
       << node->item.fqdn
       << "\"}";
    return os;
}


void to_json(const std::set<Edge>& edges)
{
    std::set<std::string> done_nodes;

    std::string nodes_str;
    std::string edges_str;
    std::ostringstream nodes_oss;
    std::ostringstream edges_oss;

    for (Edge e : edges)
    {
        edges_oss << e << ",";
        if (done_nodes.count(e.source->item.fqdn) < 1)
        {
            done_nodes.insert(e.source->item.fqdn);
            if (e.source)
                nodes_oss << "" << e.source << ",";
        }
        if (done_nodes.count(e.target->item.fqdn) < 1)
        {
            done_nodes.insert(e.target->item.fqdn);
            if (e.target)
                nodes_oss << "" << e.target << ",";
        }
    }
    nodes_str = "[" + nodes_oss.str().substr(0, nodes_oss.str().size() - 1) + "]";
    edges_str = "[" + edges_oss.str().substr(0, edges_oss.str().size() - 1) + "]";

    std::string res = "{\"nodes\":" + nodes_str + ",\"edges\":" + edges_str + "}";
    std::cout << res << std::endl;

}


void fill_urls(std::set<url_t>& urls, const std::map<std::string, URLStat>& stats)
{
    for (auto stat : stats)
    {
        urls.insert(stat.first);
    }
    return;
}
