#include "io.h"

#include <fstream>
#include <sstream>
#include <ctime>


std::string delimiter("\",\"");
Item line_to_item(std::string& line);
Transaction item_to_transaction(const Item& item);


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
    std::string url = item.fqdn;
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(url);

    Transaction res;

    while(std::getline(iss, token, '.'))
    {
        Item i(item);
        i.token = token;
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
    tokens.push_back(line.substr(0, line.size()-2)); // remove last quote

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