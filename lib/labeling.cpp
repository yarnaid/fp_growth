#include "labeling.h"

#include "../utils/utils.h"
#include <ostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>


class MeanFreq;

StatData stat_tree(const node_ptr& root);
void print_stat_table(std::vector<StatData>& stats);
std::ostream& operator<<(std::ostream& os, const StatData& stat);
std::ostream& operator<<(std::ostream& os, const stat_map& stat);
double mean_freq(const node_ptr& root);

std::vector<std::string> get_stat_keys(const std::map<std::string, double>& m)
{
    std::vector<std::string> res;
    res = get_map_keys<std::string, double>(m);
    return res;
}

void labeling(FPTree& fptree)
{
    if (fptree.empty())
    {
        return;
    }
    std::vector<node_ptr> centers = fptree.root->children;
    std::vector<StatData> stats(centers.size());

    // don't know what it for, but can be usefull
    for (int i = 0; i < centers.size(); ++i)
    {
        centers[i]->label = i;
    }

    for (auto center : centers)
    {
        stats.push_back(stat_tree(center));
    }

    print_stat_table(stats);
    return;
}

double mean_freq(const node_ptr& root)
{
    return -1;
}

class iCalculatable
{
public:
    iCalculatable();
    virtual double operator()();
};

class WalkWithParam
{
private:
    node_ptr _root;
    virtual double _process_node(const node_ptr& node) = 0;
public:
    void init()
    {
        std::queue<node_ptr> q;
        q.push(this->_root);
        while (!q.empty())
        {
            node_ptr node = q.front();
            q.pop();
            this->_process_node(node);
            if (!node->children.empty())
                for (auto child : node->children)
                    q.push(child);
        }
    }
    WalkWithParam(const node_ptr& root)
        :_root(root)
    {}
    virtual double operator()() const = 0;
};

class MeanFreq : public WalkWithParam
{
private:
    int _size;
    int _sum;
    double _process_node(const node_ptr& node)
    {
        this->_sum += node->frequency;
        this->_size += 1;
        return this->_sum;
    }
public:
    MeanFreq(const node_ptr& root)
        :_sum(-1), _size(0), WalkWithParam(root)
    {
        this->init();
    }
    inline double operator()() const {return this->_size > 0 ? double(this->_sum) / double(this->_size) : -1;}
};

class MaxFreq : public WalkWithParam
{
private:
    int _max;
    double _process_node(const node_ptr &node)
    {
        this->_max = std::max(this->_max, int(node->frequency));
        return this->_max;
    };
public:
    MaxFreq(const node_ptr& root)
        :_max(0), WalkWithParam(root)
    {
        this->init();
    }
    inline double operator ()() const {return this->_max;}
};


void print_stat_table(std::vector<StatData>& stats)
{
    if( stats.size() < 1)
        return;

    for (auto stat : stats)
    {
        if (node_ptr node = stat.node)
            std::cout << node->item.fqdn << " ";
        std::cout << stat << std::endl;
    }
    return;
}

std::ostream& operator<<(std::ostream& os, const stat_map& stat)
{
    for (auto it : stat)
    {
        os << "(" << it.first << " " << it.second << ")";
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const StatData& stat)
{
    os << stat.stat;
    return os;
}

StatData stat_tree(const node_ptr& root)
{
    StatData res;
    std::string key = "frequency";

    res.node = root;
    res.stat[key] = root->frequency;

    res.stat["label"] = double(root->label);

    MeanFreq mf(root);
    res.stat["mean_freq"] = mf();

    MaxFreq max_freq(root);
    res.stat["max_freq"] = max_freq();

    return res;
}

