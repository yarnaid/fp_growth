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
    std::string filename("data/malurl_1d_1000.csv");

    read_transactions(filename, transactions, items);

    const unsigned minimum_support_treshold = 1;
    FPTree fptree{ transactions, minimum_support_treshold };
    std::set<Pattern> patterns = fptree_growth( fptree );
    // print_pattern(patterns);

    // RESULT 1
    // train(patterns);

    // RESULT 2
    std::cout << "RESULT2" << std::endl;
    std::map<std::string, URLStat> labeled = labeling(fptree);

    // RESULT 3
    build_graph(labeled);

    return 0;
}
