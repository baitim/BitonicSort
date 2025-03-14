#include "BitonicSort/cl/bitonic_sort.hpp"
#include <filesystem>

int main(int argc, char* argv[]) try {

    std::filesystem::path exe_path(argv[0]);
    std::string dir = exe_path.parent_path().string() + '/';
    std::string kernel_file = dir + "bitonic_sort.cl";

    std::vector<int> data;
    bitonic_sort::read_data(std::cin, std::back_inserter(data));

    bitonic_sort::bitonic_sort_gpu(data.begin(), data.end(), kernel_file);

    for (auto i : data)
        std::cout << i << ' ';
    std::cout << '\n';

} catch (const bitonic_sort::error_t& error) {
    std::cout << error.what() << '\n';
    return 1;
} catch (...) {
    std::cout << print_red("Unknown error\n");
    return 1;
}