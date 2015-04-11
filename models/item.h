#ifndef ITEM_H
#define ITEM_H

#include <ctime>
#include <string>
#include <ostream>


struct Item
{
public:
    time_t time;
    std::string id;
    std::string source;
    std::string category;
    std::string name;
    std::string md5;
    std::string ip;
    std::string url;
    std::string fqdn;
    std::string asn;
    std::string cc;
    std::string details;

    int svm_class;
    std::string token;

    int q_number;
    int domain_length;
    int q_length;
    int url_length;


public:
    bool operator==(const Item& other) const {
        return (token == other.token);
    }

    bool operator<(const Item& other) const {
        return token < other.token;
    }
};

std::ostream& operator<<(std::ostream&, const Item&);



#endif // ITEM_H
