#ifndef LABELING_H
#define LABELING_H

#include <map>
#include <string>
#include <vector>
#include "../utils/utils.h"
#include "../utils/io.h"

#include "../FP-growth/fptree.hpp"

const std::string separator = ",";

struct URLStat;
std::map<std::string, URLStat> labeling(const FPTree& fptree,
                                        const std::string& file_name);

using node_ptr = std::shared_ptr<FPNode>;


struct URLStat
{
    std::set<node_ptr> labels;
    std::set<node_ptr> tokens;
    std::set<std::string> url_tokens;
    std::string my_url;

    int get_my_tokens_in_campaign() const {return tokens.size();}
    std::vector<int> get_my_tokens_depth() const;
    std::set<std::string> split_my_url(const std::string& url) {
        this->my_url = url;
        std::vector<std::string> r = url_to_tokens(url);
        for (const std::string s : r)
            this->url_tokens.insert(s);
        return this->url_tokens;
    }
    void set_labels();
};

#endif // LABELING_H
