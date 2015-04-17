#ifndef PSEUDODB_H
#define PSEUDODB_H

#include <vector>
#include <set>
#include <map>
#include <memory>

#include "../models/item.h"
#include "../models/ip.h"
#include "../models/url.h"


typedef std::shared_ptr<const Item> item_ptr;

struct PseudoDB
{
    std::map<url_t, std::set<item_ptr>> url_index;
    std::map<ip_t, std::set<item_ptr>> ip_index;
public:
    PseudoDB(const std::vector<Item>& items);
    inline std::set<item_ptr> select_url(const url_t& url) { return url_index[url]; }
    inline std::set<item_ptr> select_ip (const  ip_t&  ip) { return ip_index[ip]; }
};

#endif // PSEUDODB_H
