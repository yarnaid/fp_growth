#include "io.h"

#include <fstream>
#include <sstream>
#include <ctime>
#include <iterator>
#include <vector>
#include <string>
#include <regex>
#include <set>
//#include "../uri-parser/UriParser.hpp"


std::string delimiter("\",\"");
Item line_to_item(std::string& line);
Transaction item_to_transaction(const Item& item);

std::set<std::string> not_tokens {
    ".exe",
    ".php",
    ".org",
    ".com",
    ".html",
    ".xhtml",
    ".bat"
};

std::vector<std::string> split_string(const std::string& s, const char& delim) {
    std::vector<std::string> res;
    if (s.compare("") == 0)
    {
        return res;
    }

    std::string part;
    std::istringstream iss(s);

    while (std::getline(iss, part, delim))
    {
        res.push_back(part);
    }
    return res;
}


int read_transactions(const std::string& filename,
                      std::vector<Transaction>& transactions,
                      std::vector<Item>& items)
{
    int res = 1;
    std::ifstream input_stream(filename);
    std::string line;

    // lines to transactions
    std::getline(input_stream, line);
    while (std::getline(input_stream, line))
    {
        Item item = line_to_item(line);

        items.push_back(item);
        transactions.push_back(item_to_transaction(item));
    }

    return res;
}

std::string token_filter(const std::string& s)
{
    return s; // FIX: add real filter by extensoin
}

std::vector<std::string> url_to_tokens(const std::string& url,
                                       unsigned& q_number,
                                       unsigned& q_length,
                                       unsigned& domian_length,
                                       unsigned& url_length)
{
    std::vector<std::string> tokens;
    std::vector<std::string> res;
    std::string current_url(url);
    if (url.length() < 1)
        return tokens;

    //remove schema
    std::regex rg("(^\\w+(?=:\/\/))(.*)", std::regex::ECMAScript);
    std::smatch match;
    std::regex_search(url, match, rg);
    current_url = match[2];
    current_url = current_url.substr(3);
    url_length = current_url.length();

    //sharped tail to token
    std::size_t sharp_pos = current_url.find("#");
    if (sharp_pos != std::string::npos)
    {
        tokens.push_back(current_url.substr(sharp_pos + 1));
        current_url = current_url.substr(0, sharp_pos);
    }

    // remove ?params
    std::size_t q_mark_pos = current_url.find("?");
    if(q_mark_pos != std::string::npos)
    {
        for (auto param : split_string(current_url.substr(q_mark_pos + 1), '&'))
        {
            for (auto q : split_string(param, '='))
            {
                std::string field = token_filter(q);
                tokens.push_back(field);
                q_number += 1;
                q_length += field.length();
            }
        }
        current_url = current_url.substr(0, q_mark_pos);
    }

    //split by delims

    std::vector<std::string> by_slash;
    std::regex slash_rg = std::regex("([^\/]+(?=\/))(.*)", std::regex::ECMAScript);
    std::regex_search(current_url, match, slash_rg);

    // split domain and remove org/com...
    by_slash = split_string(match[1], '.');
    by_slash.pop_back();
    for (auto token : by_slash)
    {
        tokens.push_back(token);
        domian_length += token.length();
    }
    current_url = std::string(match[2]).substr(1); // remove leading slash

    std::size_t slash_pos;
    while(std::regex_search(current_url, match, std::regex("([\\w\.\-]+)", std::regex::ECMAScript)))
    {
        //        std::cout << match[1] << ", ";
        tokens.push_back(token_filter(match[1]));
        current_url = match.suffix().str();
    }
//    for (auto tok : tokens)
//    {
//        res.erase(res.)
//    }

    return tokens;
}

std::vector<std::string> url_to_tokens(const std::string& url)
{
    unsigned q_number;
    unsigned q_length;
    unsigned domian_length;
    unsigned url_length;
    return url_to_tokens(url, q_number, q_length, domian_length, url_length);
}


Transaction item_to_transaction(const Item& item)
{
    Transaction res;
    url_t url = item.url;

    std::vector<std::string> tokens;


    unsigned q_length = 0;
    unsigned q_number = 0;
    unsigned domain_legnth = 0;
    unsigned url_length = 0;
    tokens = url_to_tokens(url, q_number, q_length, domain_legnth, url_length);

    for (const std::string t : tokens)
    {
        Item i(item);
        i.token = t;
        i.q_length = q_length;
        i.q_number = q_number;
        i.domain_length = domain_legnth;
        i.url_length = url_length;
        res.push_back(i);
    }

    return res;
}


Item line_to_item(std::string& line)
{
    size_t pos = 0;
    std::string token;
    std::vector<std::string> tokens;

    // time id source category name md5 ip url fqdn asn cc details
    line.erase(line.begin()); // remove first quote
    while ((pos = line.find(delimiter)) != std::string::npos) {
        token = line.substr(0, pos);
        tokens.push_back(token);
        line.erase(0, pos + delimiter.length());
    }
    tokens.push_back(line.substr(0, line.size() - 2)); // remove last quote

    struct tm tm;
    strptime(tokens[0].c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
    Item res {
        mktime(&tm),
                tokens[1],
                tokens[2],
                tokens[3],
                tokens[4],
                tokens[5],
                tokens[6],
                tokens[7],
                tokens[8],
                tokens[9],
                tokens[10],
                tokens[11]
    };
    return res;
}

std::string operator"" _quoted(const char* text, std::size_t len)
{
    return "\"" + std::string(text) + "\"";
}

std::string quote(const std::string& s)
{
    return "\"" + s + "\"";
}

std::string quote(const double& s)
{
    std::ostringstream oss;
    oss << s;
    return "\"" + oss.str() + "\"";
}


std::string quote(const std::vector<int>& s)
{
    if (s.size() < 1)
        return "\"\"";
    std::ostringstream oss;
    std::copy(s.begin(), s.end() - 1, std::ostream_iterator<int>(oss, ","));
    oss << s.back();
    return "\"" + oss.str() + "\"";
}
