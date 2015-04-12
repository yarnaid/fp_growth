#include "item.h"
#include <string>
#include <vector>

std::string delim = "\",\"";
std::string delim_bound = "\"";

std::vector<std::string> item_fields = {
        "id",
        "source",
        "category",
        "name",
        "md5",
        "ip",
        "url",
        "fqdn",
        "asn",
        "cc",
        "details",
        "q_number",
        "domain_length",
        "q_length",
        "url_length",
        "svm_class"
        };

std::ostream& operator<<(std::ostream& os, const Item& item) {
    os << delim_bound
       << item.time
       << delim << item.id
       << delim << item.source
       << delim << item.category
       << delim << item.name
       << delim << item.md5
       << delim << item.ip
       << delim << item.url
       << delim << item.fqdn
       << delim << item.asn
       << delim << item.cc
       << delim << item.details
       << delim << item.q_number
       << delim << item.domain_length
       << delim << item.q_length
       << delim << item.url_length
       << delim << item.svm_class
       << delim_bound;
    return os;
}
