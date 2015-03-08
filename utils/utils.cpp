#include "utils.h"

#include <iostream>
#include <sstream>
#include <istream>
#include <vector>
#include <string>
#include <map>

std::vector<std::string> split_url(const std::string& url) {
    std::vector<std::string> res;
    std::string token;
    std::istringstream iss(url);

    while (std::getline(iss, token, '.')) {
        res.push_back(token);
    }
    res.pop_back(); // remove com,org,...

    return res;
}

template<class K, class V>
std::vector<K> get_map_keys(const std::map<K, V>& m)
{
    std::vector<K> res;
    for (auto item : m)
    {
        res.push_back(item.first);
    }
    return res;
}

template std::vector<std::string> get_map_keys <std::string, double> (const std::map<std::string, double>&);
