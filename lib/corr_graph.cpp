#include "corr_graph.h"

#include <string>
#include <set>
#include <vector>
#include <map>
#include <memory>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../lib/pseudodb.h"
#include "../models/ip.h"
#include "../models/url.h"
#include "../models/fpnode.h"
#include "../FP-growth/fptree.hpp"
//#include "../lib/graph_correlation.h"

typedef std::shared_ptr<FPNode> node_ptr;

struct Edge;
std::ostream& operator<<(std::ostream& os, const Edge& edge);
void to_json(const std::set<Edge>& edges, const std::string &json_name);

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
        return (this->source->item.id == other.source->item.id) &&
               (this->target->item.id == other.target->item.id);
    }
};


void graph(const FPTree fptree, PseudoDB& db, const std::string& json_name)
{
    std::set<FPNode> graph_nodes;
    std::map<url_t, std::set<node_ptr>> graph_urls;
    std::map<ip_t, std::set<node_ptr>> graph_ips;

    // go throw tree to get urls & ips
    std::queue<node_ptr> q;
    q.push(fptree.root);
    while (!q.empty())
    {
        node_ptr node = q.front();
        q.pop();
        for (node_ptr child : node->children)
            q.push(child);
        graph_nodes.insert(*node);
        graph_urls[node->item.url].insert(node);
        graph_ips[node->item.ip].insert(node);
    }

    // get all ips and urls
    unsigned old_url_number = 0, old_ip_number = 0, old_nodes_number = 0;
    unsigned url_number = graph_urls.size(), ip_number = graph_ips.size(), nodes_number = graph_nodes.size();

    while (old_ip_number != ip_number ||
           old_url_number != url_number ||
           old_nodes_number != nodes_number)
    {
        old_ip_number = ip_number;
        old_url_number = url_number;
        old_nodes_number = nodes_number;
        std::set<node_ptr> new_ips;
        std::set<node_ptr> new_urls;

        for (const FPNode& node : graph_nodes)
        {
            std::set<item_ptr> new_ip_items = db.select_ip(node.item.ip);
            std::set<item_ptr> new_url_items = db.select_url(node.item.url);
            new_ip_items.insert(new_url_items.begin(), new_url_items.end());  // merge for 'for'
            for(item_ptr item : new_ip_items)
            {
                node_ptr new_node = std::make_shared<FPNode>(FPNode(*item, nullptr));
                new_ips.insert(new_node);
                new_urls.insert(new_node);
                graph_nodes.insert(*new_node);
            }
        }
        for(node_ptr node : new_ips)
        {
            graph_ips[node->item.ip].insert(node);
            graph_urls[node->item.url].insert(node);
        }

        ip_number = graph_ips.size();
        url_number = graph_urls.size();
        nodes_number = graph_nodes.size();
    }

    // build connections
    std::set<Edge> edges;
    for (const FPNode& node : graph_nodes)
    {
        for (node_ptr url : graph_urls[node.item.url])
        {
            for (node_ptr ip : graph_ips[node.item.ip])
            {
                Edge e{url, ip};
                edges.insert(e);
            }
        }
    }

    // dump graph
    to_json(edges, json_name);
}


std::ostream& operator<<(std::ostream& os, const Edge& edge)
{
    os << "{";
    if (edge.source)
        os << "\"source\":\"" << edge.source->item.url << "\"";
    if (edge.source && edge.target)
        os << ",";
    if (edge.target)
        os << "\"target\":\"" << edge.target->item.url << "\"";
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const node_ptr& node)
{
    os << "{\"url\":\""
       << node->item.url
       << "\", \"freq\":\""
       << node->frequency
       << "\"}";
    return os;
}


void to_json(const std::set<Edge>& edges, const std::string& json_name)
{
    std::set<std::string> done_nodes;

    std::string nodes_str;
    std::string edges_str;
    std::ostringstream nodes_oss;
    std::ostringstream edges_oss;

    for (Edge e : edges)
    {
        edges_oss << e << ",";
        if (done_nodes.count(e.source->item.url) < 1)
        {
            done_nodes.insert(e.source->item.url);
            if (e.source)
                nodes_oss << "" << e.source << ",";
        }
        if (done_nodes.count(e.target->item.url) < 1)
        {
            done_nodes.insert(e.target->item.url);
            if (e.target)
                nodes_oss << "" << e.target << ",";
        }
    }
    nodes_str = "[" + nodes_oss.str().substr(0, nodes_oss.str().size() - 1) + "]";
    edges_str = "[" + edges_oss.str().substr(0, edges_oss.str().size() - 1) + "]";

    std::string res = "{\"nodes\":" + nodes_str + ",\"edges\":" + edges_str + "}";

    std::ofstream out_file;
    out_file.open(json_name);
    if (out_file.is_open())
    {
        out_file << res << std::endl;
        out_file.close();
    } else {
        std::cout << res << std::endl;
    }
}


void tree_to_json(const FPTree& fptree, const std::string& file_name)
{
    std::queue<node_ptr> q;
    q.push(fptree.root);
    std::set<Edge> edges;
    while(!q.empty())
    {
        node_ptr node = q.front();
        q.pop();
        for (node_ptr child : node->children)
        {
            q.push(child);
            edges.insert({node, child});
        }
    }
    to_json(edges, file_name);
}
