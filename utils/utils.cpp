#include "utils.h"

#include <iostream>
#include <sstream>
#include <istream>


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

