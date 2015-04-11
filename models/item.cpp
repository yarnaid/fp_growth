#include "item.h"

std::ostream& operator<<(std::ostream& os, const Item& item) {
    os << " " << item.time
       << " " << item.id
       << " " << item.source
       << " " << item.category
       << " " << item.name
       << " " << item.md5
       << " " << item.ip
       << " " << item.url
       << " " << item.fqdn
       << " " << item.asn
       << " " << item.cc
       << " " << item.details;
    return os;
}
