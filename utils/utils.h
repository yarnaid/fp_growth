#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <map>

std::vector<std::string> split_url(const std::string& url);

template<class K, class V>
std::vector<K> get_map_keys(const std::map<K, V>& m);

#endif // UTILS_H
