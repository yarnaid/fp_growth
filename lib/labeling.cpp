#include "labeling.h"

#include "../utils/utils.h"
#include <ostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include "../utils/io.h"

void process_url(const std::pair<std::string, URLStat>& url_stat);
void print_table(const std::map<std::string, URLStat>& urls_stat,
                 const std::string& out_file);
std::ostream& operator<<(std::ostream& os, const URLStat& stat);
std::ostream& operator<<(std::ostream& os, const std::set<std::string>& ss);


void parse_tree(const node_ptr& root,
                std::map<std::string, URLStat>& urls_stat,
                std::map<std::string, std::set<std::string>>& token2urls)
{
    std::queue<node_ptr> q;
    q.push(root);
    while (!q.empty())
    {
        node_ptr node = q.front();
        q.pop();
        if (!node->children.empty())
            for (auto child : node->children)
                q.push(child);

        std::string url = node->item.url;
        urls_stat[url] = URLStat{};
        urls_stat[url].my_url = url;
        urls_stat[url].split_my_url(url);

        std::string token = node->token;
        token2urls[token].insert(url);
    }
}

std::map<std::string, URLStat> labeling(const FPTree& fptree,
                                        const std::string& file_name)
{
    if (fptree.empty())
    {
        return std::map<std::string, URLStat>();
    }
    std::vector<node_ptr> centers = fptree.root->children;
    std::map<std::string, URLStat> urls_stat;
    std::map<std::string, std::set<std::string>> token2urls;


    // tree -> urls
    parse_tree(fptree.root, urls_stat, token2urls);

    for (auto campaign : centers)
    {
        std::queue<node_ptr> q;
        q.push(campaign);
        while (!q.empty())
        {
            node_ptr node = q.front();
            q.pop();
            if (!node->children.empty())
                for (auto child : node->children)
                    q.push(child);
            std::string token = node->token;
            std::set<std::string> urls = token2urls[token];
            for (auto url : urls)
            {
                URLStat* _stat = &urls_stat[url];
                _stat->tokens.insert(node);
                _stat->labels.insert(campaign);
            }
        }
    }

    print_table(urls_stat, file_name);

    return urls_stat;
}


void print_table(const std::map<std::string, URLStat>& urls_stat,
                 const std::string& out_file)
{
    std::ofstream s;
    if (out_file.length() > 0)
    {
        s.open(out_file);
        if (s.is_open())
        {
            s << "url,tokens_depth,tokens_number" << std::endl;
            for (const std::pair<std::string, URLStat> stat : urls_stat)
            {
                s << stat.first << separator << stat.second << std::endl;
            }
            s.close();
        } else {
            std::cerr << "Warining! Cannot write stat data!" << std::endl;
            return;
        }
    }
}

std::ostream& operator<<(std::ostream& os, const std::set<std::string>& ss)
{
    for (auto s : ss)
        os << s << " ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const URLStat& stat)
{
    os << quote(stat.get_my_tokens_depth())
       << separator << stat.get_my_tokens_in_campaign();
    for (auto t : stat.tokens)
        os << separator << t->campaign;
    os << separator;
    return os;
}



std::vector<int> URLStat::get_my_tokens_depth() const
{
    std::vector<int> res;
    for (const node_ptr token : this->tokens)
        res.push_back(token->depth);
    return res;
}
