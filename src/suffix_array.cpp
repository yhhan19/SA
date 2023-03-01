#include "suffix_array.hpp"

SuffixArray::SuffixArray(const std::string &input, index_t k) {
    this->input = input + SENTINEL;
    this->size = this->input.size();
    this->k = k;
    this->sa = new index_t[size];
    this->ra = new index_t[size];
    build_sa();
    if (k > 0) {
        for (index_t i = 0; i + k < size; i ++) {
            std::string prefix = input.substr(i, k);
            auto it = pretab.find(prefix);
            if (it == pretab.end()) pretab[prefix] = std::make_pair(-1, 0);
            if (ra[i] < pretab[prefix].first) pretab[prefix].first = ra[i];
            if (ra[i] + 1 > pretab[prefix].second) pretab[prefix].second = ra[i] + 1;
        }
    }
}

SuffixArray::SuffixArray(const std::string &file) {
    std::ifstream fin(file, std::ios::binary);
    const index_t step = sizeof(index_t);
    fin.seekg(0, fin.end);
    index_t length = fin.tellg();
    fin.seekg(0, fin.beg);
    char *buffer = new char[length];
    fin.read(buffer, length);
    memcpy((char*) &size, buffer, step);
    memcpy((char*) &k, buffer + step, step);
    index_t total = step * 2;
    for (index_t i = 0; i < size; i ++) 
        input += buffer[total ++];
    this->sa = new index_t[size];
    for (index_t i = 0; i < size; i ++) {
        memcpy((char*) (sa + i), buffer + total, step);
        total += step;
    }
    this->ra = new index_t[size];
    for (index_t i = 0; i < size; i ++) 
        ra[sa[i]] = i;
    if (k > 0) {
        for (index_t i = 0; i + k < size; i ++) {
            std::string prefix = input.substr(i, k);
            auto it = pretab.find(prefix);
            if (it == pretab.end()) pretab[prefix] = std::make_pair(-1, 0);
            if (ra[i] < pretab[prefix].first) pretab[prefix].first = ra[i];
            if (ra[i] + 1 > pretab[prefix].second) pretab[prefix].second = ra[i] + 1;
        }
    }
    delete [] buffer;
    fin.close();
}

SuffixArray::~SuffixArray() {
    delete [] this->sa;
    delete [] this->ra;
}

std::string SuffixArray::to_string() {
    return to_string(false);
}

index_t SuffixArray::write(const std::string &file) {
    std::ofstream fout(file, std::ios::binary);
    const index_t step = sizeof(index_t);
    char *buffer = new char[size + (size + 2) * step];
    memcpy(buffer, (char*) &size, step);
    memcpy(buffer + step, (char*) &k, step);
    index_t total = step * 2;
    for (index_t i = 0; i < size; i ++) 
        buffer[total ++] = input[i];
    for (index_t i = 0; i < size; i ++) {
        memcpy(buffer + total, (char*) (sa + i), step);
        total += step;
    }
    fout.write(buffer, total);
    delete [] buffer;
    fout.close();
    return total;
}

std::vector<index_t> SuffixArray::search(const std::string &pattern) {
    std::pair<index_t, index_t> range;
    if (k == 0 || pattern.size() < k) {
        range = std::make_pair(0, size);
    }
    else {
        auto it = pretab.find(pattern.substr(0, k));
        if (it == pretab.end()) return std::vector<index_t>();
        range = it->second;
    }
    std::vector<index_t> result;
    index_t lower = lower_bound_simple(pattern + SENTINEL, range);
    index_t upper = lower_bound_simple(pattern + SENTINEL_GREAT, range);
    for (index_t i = lower; i < upper; i ++) 
        result.push_back(sa[i]);
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<index_t> SuffixArray::search_naive(const std::string &pattern) {
    std::pair<index_t, index_t> range;
    if (k == 0 || pattern.size() < k) {
        range = std::make_pair(0, size);
    }
    else {
        auto it = pretab.find(pattern.substr(0, k));
        if (it == pretab.end()) return std::vector<index_t>();
        range = it->second;
    }
    std::vector<index_t> result;
    index_t lower = lower_bound_naive(pattern + SENTINEL, range);
    index_t upper = lower_bound_naive(pattern + SENTINEL_GREAT, range);
    for (index_t i = lower; i < upper; i ++) 
        result.push_back(sa[i]);
    std::sort(result.begin(), result.end());
    return result;
}

void SuffixArray::build_sa() {
    index_t *tsa = new index_t[size];
    index_t *tra = new index_t[size];
    index_t s = ALPHABET > size ? ALPHABET : size;
    index_t *count = new index_t[s];
    for (index_t i = 0; i < s; i ++) count[i] = 0;
    for (index_t i = 0; i < size; i ++) count[input[i]] += 1;
    for (index_t i = 1; i < s; i ++) count[i] += count[i - 1];
    for (index_t i = size - 1; i != -1; i --) sa[-- count[input[i]]] = i;
    ra[sa[0]] = 0;
    index_t unique = 0;
    for (index_t i = 1; i < size; i ++) {
        if (input[sa[i]] != input[sa[i - 1]]) unique ++;
        ra[sa[i]] = unique;
    }
    for (index_t j = 1; unique < size - 1; j *= 2) {
        for (index_t i = 0; i < size; i ++) tra[i] = ra[i];
        for (index_t i = size - j, k = 0; i < size; i ++) tsa[k ++] = i;
        for (index_t i = 0, k = j; i < size; i ++) if (sa[i] >= j) tsa[k ++] = sa[i] - j;
        for (index_t i = 0; i < size; i ++) ra[i] = tra[tsa[i]];
        for (index_t i = 0; i < size; i ++) count[i] = 0;
        for (index_t i = 0; i < size; i ++) count[ra[i]] ++;
        for (index_t i = 1; i < size; i ++) count[i] += count[i - 1];
        for (index_t i = size - 1; i != -1; i --) sa[-- count[ra[i]]] = tsa[i];
        ra[sa[0]] = 0; unique = 0;
        for (index_t i = 1; i < size; i ++) {
            if (tra[sa[i]] != tra[sa[i - 1]] || 
            (sa[i] + j < size ? tra[sa[i] + j] : -1) != 
            (sa[i - 1] + j < size ? tra[sa[i - 1] + j] : -1)) 
                unique ++;
            ra[sa[i]] = unique;
        }
    }
    delete [] tsa;
    delete [] tra;
    delete [] count;
}

std::string SuffixArray::to_string(bool display) {
    std::string s = std::to_string(size) + ' ' + std::to_string(k) + '\n';
    for (index_t i = 0; i < size; i ++) {
        s += std::to_string(sa[i]) + ' ';
        if (display) {
            for (index_t j = sa[i]; j < size; j ++) 
                s += input[j];
            s += '\n';
        }
    }
    return s;
}

inline index_t SuffixArray::LCP(const std::string &pattern, index_t i, index_t j) {
    if (sa[i] >= size) return 0;
    while (j < pattern.size() && pattern[j] == input[sa[i] + j]) 
        j ++;
    return j;
}

index_t SuffixArray::lower_bound_naive(const std::string &pattern, std::pair<index_t, index_t> range) {
    index_t lower = range.first, upper = range.second;
    while (lower < upper) {
        index_t mid = (lower + upper) / 2, i = LCP(pattern, mid, 0);
        if (i == pattern.size() || pattern[i] <= input[sa[mid] + i]) 
            upper = mid;
        else 
            lower = mid + 1;
    }
    return lower;
}

index_t SuffixArray::lower_bound_simple(const std::string &pattern, std::pair<index_t, index_t> range) {
    index_t lower = range.first, upper = range.second;
    // index_t LCP_lower = LCP(pattern, lower, 0), LCP_upper = LCP(pattern, upper, 0);
    index_t LCP_lower = 0, LCP_upper = 0;
    while (lower < upper) {
        index_t mid = (lower + upper) / 2;
        index_t j = LCP_lower < LCP_upper ? LCP_lower : LCP_upper;
        index_t i = LCP(pattern, mid, j);
        if (i == pattern.size() || pattern[i] <= input[sa[mid] + i]) {
            upper = mid;
            LCP_upper = i;
        }
        else {
            lower = mid + 1;
            LCP_lower = i;
        }
    }
    return lower;
}
