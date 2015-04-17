#include <iostream>
#include <vector>

#include "FP-growth/fptree.hpp"

#include "utils/io.h"
#include "lib/train.h"
#include "lib/labeling.h"
#include "lib/graph_correlation.h"
#include "lib/corr_graph.h"
#include "lib/pseudodb.h"
#include "utils/campaign_marker.h"
#include "models/item.h"


int main(int argc, char const *argv[]) {
    std::vector<Transaction> transactions;
    std::vector<Item> items;
    std::string filename("data/malurl_1d_20000.csv");

    read_transactions(filename, transactions, items);

    const double minimum_support_treshold = 0.03;
    const double maximum_supprot_treshold = 0.99;
    const unsigned class_treshold = 3;
    FPTree fptree{ transactions, minimum_support_treshold, maximum_supprot_treshold };
    std::set<Pattern> patterns = fptree_growth( fptree, class_treshold );
//    print_pattern(patterns);
    dump_pattern(patterns, "./learning.csv");  // save learning set to file
    PseudoDB db(items);

    // RESULT 1
    std::cout << "RESULT1" << std::endl;
    mark_tree(fptree, false);  // mark campaigns and save them to files
    std::vector<Item> test_vector;
    unsigned test_number = 100;
    test_vector.insert(test_vector.end(), items.begin(), items.begin() + test_number);
    build_classifier(patterns, test_vector, 0.8, 3);

    // RESULT 2
    std::cout << "RESULT2" << std::endl;
    std::map<std::string, URLStat> labeled = labeling(fptree, std::string(""));

    // RESULT 3
    std::cout << "RESULT3" << std::endl;
    graph(fptree, db, "./graph_vis/graph.json");
    tree_to_json(fptree, "./graph_vis/tree.json");

    return 0;
}
