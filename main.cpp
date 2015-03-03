#include <iostream>
#include <vector>

#include "FP-growth/fptree.hpp"


void test_fp()
{
    /* code */
    const Item a{10, "a"};
    const Item b{2, "b"};
    const Item c{3, "c"};
    const Item d{4, "d"};
    const Item e{5, "e"};
    const Item f{6, "f"};

    const std::vector<Transaction> transactions{
        { a, b },
        { b, c, d },
        { a, c, d, e },
        { a, d, e },
        { a, b, c },
        { a, b, c, d },
        { a },
        { a, b, c },
        { a, b, d },
        { b, c, e }
    };

    const unsigned minimum_support_treshold = 2;

    const FPTree fptree{ transactions, minimum_support_treshold };

    const std::set<Pattern> patterns = fptree_growth( fptree );

    print_pattern(patterns);
}

int main(int argc, char const *argv[])
{
    /* code */
    test_fp();
    return 0;
}