#include "io.h"

#include <fstream>
#include <sstream>
#include <ctime>
#include <iterator>
#include <vector>
#include <string>
#include "../uri-parser/UriParser.hpp"


std::string delimiter("\",\"");
Item line_to_item(std::string& line);
Transaction item_to_transaction(const Item& item);

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


Transaction item_to_transaction(const Item& item)
{
    url_t url = item.url;
    std::string token;
    std::istringstream iss(url);

    Transaction res;

//    std::string schema(parse_by_regex(std::regex("(^\\w+(?=:\/\/))?(:\/\/)?(www(?=\.))?(\.)?(\w+)"), url)[0]);
    http::url parsed = http::ParseHttpUrl(url);
    std::string protocol = parsed.protocol;
    std::string user = parsed.user;
    std::string password = parsed.password;
    std::string host = parsed.host;
    int port = parsed.port;
    std::string path = parsed.path;
    std::string query = parsed.search;

    std::vector<std::string> splitted_host = split_string(host, '.');
    std::vector<std::string> splitted_path = split_string(path, '/');
    std::vector<std::string> splitted_query = split_string(query, '&');

    std::string sharped("");
    if (!splitted_query.empty())
    {
        std::vector<std::string> q = split_string(splitted_query.back(), '#');
        if (q.size() > 1)
        {
//            url must not contain more than 1 sharp
            splitted_query.back() = q[0];
            sharped = q[1];
        }
    }


    std::vector<std::string> tokens;

    tokens.reserve(splitted_host.size() + splitted_path.size() + splitted_query.size() + 1);
    tokens.insert(tokens.end(), splitted_host.begin(), splitted_host.end() - 1); // remove org, com...
    tokens.insert(tokens.end(), splitted_path.begin(), splitted_path.end());
    tokens.insert(tokens.end(), splitted_query.begin(), splitted_query.end());
    if (sharped.compare("") != 0)
    {
        tokens.push_back(sharped);
    }

    for (const std::string t : tokens)
    {
        Item i(item);
        i.token = t;
        i.q_number = splitted_query.size();
        i.domain_length = path.length();
        i.q_length = query.length() - sharped.length();
        i.url_length = url.length();
        res.push_back(i);
    }
    if (!res.empty())
        res.pop_back(); // remove com,org,...

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
