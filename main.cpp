#include <iostream>
#include <vector>

#include "FP-growth/fptree.hpp"

#include "utils/io.h"
#include "lib/train.h"
#include "lib/labeling.h"
#include "lib/graph_correlation.h"
#include "utils/campaign_marker.h"


int main(int argc, char const *argv[]) {
    std::vector<Transaction> transactions;
    std::vector<Item> items;
    std::string filename("data/malurl_1d_10000.csv");

    read_transactions(filename, transactions, items);

    const double minimum_support_treshold = 3;
    const double maxumum_supprot_treshold = 0.5;
    const unsigned class_treshold = 3;
    FPTree fptree{ transactions, minimum_support_treshold, maxumum_supprot_treshold };
    std::set<Pattern> patterns = fptree_growth( fptree, class_treshold );
//    print_pattern(patterns);
//    dump_pattern(patterns, "./learning.csv");

    // RESULT 1
    std::cout << "RESULT1" << std::endl;
    train<Kernels::Sigmoid>(patterns);  // By default we use default dlib's parameters for kernels
                                        // But they can be tuned by giving alternative kernel
                                        // as second argument.

    // RESULT 2
    std::cout << "RESULT2" << std::endl;
//    mark_tree(fptree);
//    std::map<std::string, URLStat> labeled = labeling(fptree);
    // RESULT 3
    std::cout << "RESULT3" << std::endl;
    // build_graph(labeled, fptree);

    return 0;
}
