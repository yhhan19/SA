#include "test.hpp"

Test::Test() {

}

Test::Test(index_t size, index_t alphabet) {
    this->size = size;
    this->alphabet = alphabet;
    srand(time(0));
    input = "";
    for (index_t i = 0; i < size; i ++) 
        input += 'a' + rand() % alphabet;
}

Test::~Test() {

}

SuffixArray *Test::test_construct(index_t k) {
    SuffixArray *sa = new SuffixArray(input, k);
    std::string input_ = input + SENTINEL;
    std::vector<std::string> suffix;
    for (index_t i = 0; i < input_.size(); i ++) 
        suffix.push_back(input_.substr(i, input_.size() - i));
    std::sort(suffix.begin(), suffix.end());
    std::stringstream ss(sa->to_string());
    index_t temp; ss >> temp >> temp;
    for (index_t i = 0; i < input_.size(); i ++) {
        index_t a = input_.size() - suffix[i].size();
        index_t b = -1; ss >> b;
        if (a != b) {
            std::cout << "incorrect construction" << std::endl;
            return NULL;
        }
    }
    std::cout << "correct construction" << std::endl;
    return sa;
}

SuffixArray *Test::test_io(SuffixArray *sa, std::string file) {
    sa->write(file);
    SuffixArray *sa_ = new SuffixArray(file);
    if (sa_->to_string() != sa->to_string()) {
        std::cout << "incorrect io" << std::endl;
        std::cout << sa->to_string() << std::endl;
        std::cout << sa_->to_string() << std::endl;
        return NULL;
    }
    std::cout << "correct io" << std::endl;
    return sa_;
}

bool Test::test_search(SuffixArray *sa, index_t t, index_t size) {
    index_t max = 0, min = -1;
    for (index_t k = 0; k < t; k ++) {
        std::string pattern = "";
        for (index_t i = 0; i < size; i ++) 
            pattern += 'a' + rand() % alphabet;
        std::vector<index_t> result_;
        for (index_t i = 0; i < input.size(); i ++) {
            index_t j = 0;
            while (j < pattern.size() && pattern[j] == input[i + j]) 
                j ++;
            if (j == pattern.size()) 
                result_.push_back(i);
        }
        std::vector<index_t> result = sa->search(pattern);
        if (result.size() != result_.size()) {
            std::cout << "incorrect search" << std::endl;
            return false;
        }
        for (index_t i = 0; i < result.size(); i ++) {
            if (result[i] != result_[i]) {
                std::cout << "incorrect search" << std::endl;
                return false;
            }
        }
        if (result.size() > max) max = result.size();
        if (result.size() < min) min = result.size();
    }
    std::cout << "correct search: " << min << ' ' << max << std::endl;
    return true;
}

bool Test::test(index_t k, std::string file, index_t t, index_t size) {
    SuffixArray *sa = test_construct(k);
    if (sa == NULL) {
        delete sa;
        return false;
    }
    SuffixArray *sa_ = test_io(sa, file);
    if (sa_ == NULL) {
        delete sa_;
        return false;
    }
    bool final = test_search(sa_, t, size);
    delete sa;
    delete sa_;
    return final;
}

void Test::test_reference_input(std::string file, std::string index_file) {
    srand(time(0));
    char alphabet[4] = {'A', 'C', 'G', 'T'};
    std::vector<index_t> sizes({1000000, 2000000});
    input = "";
    std::ifstream fin(file);
    std::string row, name = "", sequence = "";
    while (std::getline(fin, row)) {
        if (row[0] != '>') {
            sequence += row;
        }
        else {
            name = row.substr(1, row.size() - 1);
            input += sequence;
            if (input.size() >= sizes[sizes.size() - 1]) break;
            sequence = "";
        }
    }
    std::cout << input.size() << std::endl;
    fin.close();
    std::vector<index_t> pattern_sizes({20, 40, 80, 160, 320});
    index_t queries = 1000000;
    for (index_t size : sizes) {
        std::vector<std::string> patterns;
        for (index_t i = 0; i < size; i += size / queries) {
            std::string pattern = "";
            for (index_t j = 0; j < pattern_sizes[pattern_sizes.size() - 1]; j ++) {
                pattern += input[i + j];
            }
            patterns.push_back(pattern);
        }
        for (index_t k = 0; k <= 16; k += 4) {
            std::string input_ = input.substr(0, size);
            auto start = std::chrono::high_resolution_clock::now();
            SuffixArray sa(input_, k);
            auto end = std::chrono::high_resolution_clock::now();
            auto construct_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            index_t file_size = sa.write(index_file);
            SuffixArray sa_(index_file);
            std::cout << size << ' ' << k;
            std::cout << ' ' << construct_duration.count() << ' ' << file_size << std::endl;
            for (index_t psize : pattern_sizes) {
                std::vector<std::string> patterns_;
                for (std::string pattern : patterns) 
                    patterns_.push_back(pattern.substr(0, psize));
                start = std::chrono::high_resolution_clock::now();
                for (std::string pattern : patterns_) {
                    std::vector<index_t> result = sa_.search(pattern);
                    // std::cout << pattern << ' ';
                    // std::cout << result.size() << std::endl;
                }
                end = std::chrono::high_resolution_clock::now();
                auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                start = std::chrono::high_resolution_clock::now();
                for (std::string pattern : patterns_) 
                    std::vector<index_t> result = sa_.search_naive(pattern);
                end = std::chrono::high_resolution_clock::now();
                auto search_naive_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                std::cout << psize;
                std::cout << ' ' << search_duration.count() << ' ' << search_naive_duration.count();
                std::cout << std::endl;
            }
        }
    }
}

void Test::test_reference_random(std::string index_file) {
    srand(time(0));
    char alphabet[4] = {'A', 'C', 'G', 'T'};
    std::vector<index_t> sizes({1000000, 2000000, 3000000, 4000000});
    input = "";
    for (index_t i = 0; i < sizes[sizes.size() - 1]; i ++) 
        input += alphabet[rand() % 4];
    std::vector<index_t> pattern_sizes({10, 20, 40, 80, 160, 320});
    index_t queries = 1000000;
    for (index_t size : sizes) {
        std::vector<std::string> patterns;
        for (index_t i = 0; i < size; i += size / queries) {
            std::string pattern = "";
            for (index_t j = 0; j < pattern_sizes[pattern_sizes.size() - 1]; j ++) {
                pattern += input[i + j];
            }
            patterns.push_back(pattern);
            // std::cout << pattern << std::endl;
        }
        for (index_t k = 0; k <= 16; k += 4) {
            std::string input_ = input.substr(0, size);
            auto start = std::chrono::high_resolution_clock::now();
            SuffixArray sa(input_, k);
            auto end = std::chrono::high_resolution_clock::now();
            auto construct_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            index_t file_size = sa.write(index_file);
            SuffixArray sa_(index_file);
            std::cout << size << ' ' << k;
            std::cout << ' ' << construct_duration.count() << ' ' << file_size << std::endl;
            for (index_t psize : pattern_sizes) {
                std::vector<std::string> patterns_;
                for (std::string pattern : patterns) 
                    patterns_.push_back(pattern.substr(0, psize));
                start = std::chrono::high_resolution_clock::now();
                for (std::string pattern : patterns_) {
                    std::vector<index_t> result = sa_.search(pattern);
                    // std::cout << pattern << ' ';
                    // std::cout << result.size() << std::endl;
                }
                end = std::chrono::high_resolution_clock::now();
                auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                start = std::chrono::high_resolution_clock::now();
                for (std::string pattern : patterns_) 
                    std::vector<index_t> result = sa_.search_naive(pattern);
                end = std::chrono::high_resolution_clock::now();
                auto search_naive_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                std::cout << psize;
                std::cout << ' ' << search_duration.count() << ' ' << search_naive_duration.count();
                std::cout << std::endl;
            }
        }
    }
}

std::string Test::to_string() {
    return input;
}
