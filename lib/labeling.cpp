#include "labeling.h"

#include "../utils/utils.h"
#include <ostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include "../utils/io.h"


StatData stat_tree(const node_ptr& root, const int& label);
void print_stat_table(const std::vector<StatData>& stats);
std::ostream& operator<<(std::ostream& os, const StatData& stat);
std::ostream& operator<<(std::ostream& os, const stat_map& stat);
double mean_freq(const node_ptr& root);


std::vector<StatData> labeling(FPTree& fptree)
{
    if (fptree.empty())
    {
        return std::vector<StatData>();
    }
    std::vector<node_ptr> centers = fptree.root->children;
    std::vector<StatData> stats;

    for (int i = 0; i < centers.size(); ++i)
    {
        StatData s = stat_tree(centers[i], i);
        stats.push_back(s);
    }


    print_stat_table(stats);
    return stats;
}


class iCalculatable
{
public:
    std::string key;

    iCalculatable(const std::string& key) :key(key) {}
    virtual double operator()(const node_ptr& node) = 0;
    virtual double get() const = 0;
};

using calc_ptr = std::shared_ptr<iCalculatable>;

class MaxFreq : public iCalculatable
{
private:
    double _max;
public:
    MaxFreq() :_max(-1), iCalculatable("max_freq") {}
    inline double operator ()(const node_ptr& node)
    {
        return this->_max = std::max(this->_max, double(node->frequency));
    }
    double get() const {return this->_max;}
};

class NodeCount : public iCalculatable
{
private:
    unsigned _count;
public:
    NodeCount() :_count(0), iCalculatable("descendant") {}
    inline double operator ()(const node_ptr& node) {return double(this->_count++);}
    double get() const {return double(this->_count);}
};

class MaxDepth : public iCalculatable
{
private:
    unsigned _depth;
public:
    MaxDepth() : _depth(0), iCalculatable("max_depth") {}
    double operator ()(const node_ptr& node)
    {
        unsigned depth = 0;
        node_ptr current = node;
        while (current->parent)
        {
            current = current->parent;
            ++depth;
        }
        return double(this->_depth = std::max(this->_depth, depth));
    }
    double get() const {return double(this->_depth);}
};

class Mean : public iCalculatable
{
private:
    double _sum;
    int _size;
public:
    Mean() :_sum(-1), _size(0), iCalculatable("mean_freq") {}
    double operator ()(const node_ptr& node)
    {
        this->_sum += node->frequency;
        return (this->_size)++;
    }
    double get() const { return this->_size>0? this->_sum / this->_size:-1; }

};

class StatWalk
{
private:
    node_ptr _root;
    std::vector<calc_ptr> _stat_functions;
public:
    void init()
    {
        std::queue<node_ptr> q;
        q.push(this->_root);
        while (!q.empty())
        {
            node_ptr node = q.front();
            q.pop();
            for (auto stat : this->_stat_functions)
                (*stat)(node);
            if (!node->children.empty())
                for (auto child : node->children)
                    q.push(child);
        }
    }
    StatWalk(const node_ptr& root, const std::vector<calc_ptr>& stats)
        :_root(root), _stat_functions(stats) {this->init();}
    stat_map get_stat()
    {
        stat_map res;
        for (auto stat : this->_stat_functions)
            res[(*stat).key] = (*stat).get();
        return res;
    }
};


void print_stat_table(const std::vector<StatData>& stats)
{
    if( stats.size() < 1)
        return;

    std::vector<std::string> keys = get_map_keys <std::string, double> (stats.front().stat);
    std::cout << "fqdn,";
    std::copy(keys.begin(), keys.end() - 1, std::ostream_iterator<std::string>(std::cout, ","));
    std::cout << keys.back() << std::endl;


    for (auto stat : stats)
    {
        if (node_ptr node = stat.node)
            std::cout << quote(node->item.fqdn);
        std::cout << stat << std::endl;
    }
    return;
}

std::ostream& operator<<(std::ostream& os, const stat_map& stat)
{
    for (auto it : stat)
    {
        os << "," << quote(it.second);
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const StatData& stat)
{
    os << stat.stat;
    return os;
}

StatData stat_tree(const node_ptr& root, const int& label)
{
    StatData res;
    std::string freq = "frequency";

    res.node = root;

    std::shared_ptr<Mean> mean(new Mean());
    std::shared_ptr<MaxFreq> max_freq(new MaxFreq());
    std::shared_ptr<NodeCount> node_count(new NodeCount());
    std::shared_ptr<MaxDepth> max_depth(new MaxDepth());
    std::vector<calc_ptr> stats;
    stats.push_back(mean);
    stats.push_back(max_freq);
    stats.push_back(node_count);
    stats.push_back(max_depth);

    StatWalk stat_walk(root, stats);
    res.stat = stat_walk.get_stat();

    res.stat[freq] = root->frequency;
    res.stat["label"] = label;

    return res;
}
