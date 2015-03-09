#ifndef GRAPHCORRELATION_H
#define GRAPHCORRELATION_H

#include "labeling.h"

#include <map>
#include <string>
#include <set>

using url_t = std::string;
using ip_t = std::string;

using mapping = std::map<std::string, std::set<std::string>>;
//mapping url2ip, ip2url;

void build_graph(const std::map<std::string, URLStat>& stats, const FPTree& fptree);

#endif // GRAPHCORRELATION_H
