#ifndef GRAPHCORRELATION_H
#define GRAPHCORRELATION_H

#include "labeling.h"

#include <map>
#include <string>
#include <set>

#include "../models/ip.h"
#include "../models/url.h"
#include "../lib/pseudodb.h"

using mapping = std::map<std::string, std::set<std::string>>; //mapping url2ip, ip2url;

struct Edge;
std::ostream& operator<<(std::ostream& os, const Edge& edge);
void to_json(const std::set<Edge>& edges, const std::string &json_name);



void build_graph(const std::map<std::string, URLStat>& stats, const FPTree& fptree, const std::string &out_file_name, PseudoDB &db);

#endif // GRAPHCORRELATION_H
