#include "suffix_array.hpp"

int main(int argc, char **argv) {
    index_t k = 0;
    std::string input_file = "", output_file = "";
    for (index_t i = 0; i < argc; i ++) {
        std::string opt(argv[i]);
        if (opt == "--preftab") k = std::stoi(argv[++ i]);
        if (opt == "reference") input_file = std::string(argv[++ i]);
        if (opt == "output") output_file = std::string(argv[++ i]);
    }
    std::ifstream fin(input_file);
    std::string row, name = "", sequence = "";
    while (std::getline(fin, row)) {
        if (row[0] != '>') {
            sequence += row;
        }
        else {
            if (name != "") break;
            name = row.substr(1, row.size() - 1);
            sequence = "";
        }
    }
    fin.close();
    SuffixArray sa(sequence, k);
    sa.write(output_file);
    return 0;
}
