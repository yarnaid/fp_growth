#include <iostream>
#include <vector>

#include "FP-growth/fptree.hpp"

#include "utils/io.h"
#include "lib/train.h"
#include "lib/labeling.h"
#include "lib/graph_correlation.h"


int main(int argc, char const *argv[]) {
    std::vector<Transaction> transactions;
    std::vector<Item> items;
    std::string filename("data/malurl_1d_10000.csv");

    read_transactions(filename, transactions, items);

    const double minimum_support_treshold = 0.1;
    const double maxumum_supprot_treshold = 0.9;
    FPTree fptree{ transactions, minimum_support_treshold, maxumum_supprot_treshold };
    std::set<Pattern> patterns = fptree_growth( fptree );
    print_pattern(patterns);
    return 0;

    // RESULT 1
    std::cout << "RESULT1" << std::endl;
    train(patterns);

    // RESULT 2
    std::cout << "RESULT2" << std::endl;
    // std::map<std::string, URLStat> labeled = labeling(fptree);

    // RESULT 3
    std::cout << "RESULT3" << std::endl;
    // build_graph(labeled, fptree);

    return 0;
}
