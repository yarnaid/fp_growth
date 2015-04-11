#ifndef GRAPHCORRELATION_H
#define GRAPHCORRELATION_H

#include "labeling.h"

#include <map>
#include <string>
#include <set>

#include "../models/ip.h"
#include "../models/url.h"

using mapping = std::map<std::string, std::set<std::string>>;
//mapping url2ip, ip2url;

void build_graph(const std::map<std::string, URLStat>& stats, const FPTree& fptree);

#endif // GRAPHCORRELATION_H
