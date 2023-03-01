#include "suffix_array.hpp"
#include "test.hpp"

int main() {
    /*
    Test test(50000, 2);
    test.test(4, "./bin/index", 10000, 12);
    */
    Test test;
    // test.test_reference_random("./bin/index");
    test.test_reference_input("./bin/GCF_001839005.1_ASM183900v1_genomic.fna", "./bin/index2");
    return 0;
}
