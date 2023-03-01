#ifndef SA_HPP
#define SA_HPP

#include "utility.hpp"

class SuffixArray {
    public:
        SuffixArray(const std::string &input, index_t k);
        SuffixArray(const std::string &file);
        ~SuffixArray();
        std::string to_string();
        index_t write(const std::string &file);
        std::vector<index_t> search(const std::string &pattern);
        std::vector<index_t> search_naive(const std::string &pattern);
    private:
        index_t size, k, *sa, *ra;
        std::string input;
        std::unordered_map<std::string, std::pair<index_t, index_t>> pretab;
        void build_sa();
        std::string to_string(bool display);
        index_t LCP(const std::string &pattern, index_t i, index_t j);
        index_t lower_bound_naive(const std::string &pattern, std::pair<index_t, index_t> range);
        index_t lower_bound_simple(const std::string &pattern, std::pair<index_t, index_t> range);
};

#endif
