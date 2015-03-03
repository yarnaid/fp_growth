#include <iostream>
#include <vector>

#include "FP-growth/fptree.hpp"


void test_fp()
{
    /* code */
    const Item a{1};
    const Item b{2};
    const Item c{3};
    const Item d{4};
    const Item e{5};
    const Item f{6};

    // const Item a{ "A" };
    // const Item b{ "B" };
    // const Item c{ "C" };
    // const Item d{ "D" };
    // const Item e{ "E" };

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
}

int main(int argc, char const *argv[])
{
    /* code */
    test_fp();
    return 0;
}