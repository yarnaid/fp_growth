#include "pseudodb.h"

PseudoDB::PseudoDB(const std::vector<Item> &items)
{
    for (auto item : items)
    {
        url_index[item.url].insert(std::make_shared<Item>(item));
        ip_index[item.ip].insert(std::make_shared<Item>(item));
    }
}
