#include "suffix_array.hpp"

int main(int argc, char **argv) {
    std::string index_file = "", query_file = "", mode = "", output_file = "";
    for (index_t i = 0; i < argc; i ++) {
        std::string opt(argv[i]);
        if (opt == "index") index_file = std::string(argv[++ i]);
        if (opt == "queries") query_file = std::string(argv[++ i]);
        if (opt == "query") mode = std::string(argv[++ (++ i)]);
        if (opt == "output") output_file = std::string(argv[++ i]);
    }
    SuffixArray sa(index_file);
    std::ifstream fin(query_file);
    std::string row, name = "", sequence = "";
    std::vector<std::pair<std::string, std::string>> queries;
    while (std::getline(fin, row)) {
        if (row[0] != '>') {
            sequence += row;
        }
        else {
            if (name != "") queries.push_back(std::make_pair(name, sequence));
            name = row.substr(1, row.size() - 1);
            sequence = "";
        }
    }
    if (name != "") queries.push_back(std::make_pair(name, sequence));
    fin.close();
    std::vector<std::vector<index_t>> results;
    for (auto q : queries) {
        if (mode == "naive") 
            results.push_back(sa.search_naive(q.second));
        else 
            results.push_back(sa.search(q.second));
    }
    std::ofstream fout(output_file);
    for (index_t i = 0; i < queries.size(); i ++) {
        fout << queries[i].first << '\t' << results[i].size();
        for (index_t hit : results[i]) 
            fout << '\t' << hit;
        fout << std::endl;
    }
    fout.close();
    return 0;
}
