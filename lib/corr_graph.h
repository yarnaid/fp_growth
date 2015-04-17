#ifndef CORR_GRAPH_H
#define CORR_GRAPH_H

#include <string>
#include <set>
#include <vector>
#include <map>

#include "../lib/pseudodb.h"
#include "../models/ip.h"
#include "../models/url.h"
#include "../models/fpnode.h"
#include "../FP-growth/fptree.hpp"


void graph(const FPTree fptree, PseudoDB& db, const std::string &json_name);
void tree_to_json(const FPTree& fptree, const std::string& file_name);


#endif // CORR_GRAPH_H
