#ifndef TEST_HPP
#define TEST_HPP
#include "suffix_array.hpp"

class Test {
    public:
        Test();
        Test(index_t size, index_t alphabet);
        ~Test();
        std::string to_string();
        SuffixArray *test_construct(index_t k);
        SuffixArray *test_io(SuffixArray *sa, std::string file);
        bool test_search(SuffixArray *sa, index_t t, index_t size);
        bool test(index_t k, std::string file, index_t t, index_t size);
        void test_reference_random(std::string file);
        void test_reference_input(std::string file, std::string index_file);
    private:
        index_t size, alphabet;
        std::string input;
};

#endif