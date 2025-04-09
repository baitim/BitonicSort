#pragma once

#include "BitonicSort/ANSI_colors.hpp"
#include <sstream>
#include <fstream>

namespace bitonic_sort {
    class error_t : public std::runtime_error {
    public:
        error_t(std::string msg) : std::runtime_error(std::move(msg)) {}
    };

    inline std::string file2str(std::string_view file_name) {
        std::ifstream input_file{std::string(file_name)};
        if (input_file.is_open()) {
            std::stringstream sstr;
            sstr << input_file.rdbuf();
            input_file.close();
            return sstr.str();
        }
        throw error_t{str_red("Can't open file: " + std::string(file_name))};
    }
}