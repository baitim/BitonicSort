#include "BitonicSort/cl/bitonic_sort.hpp"
#include "benchmark.hpp"
#include <algorithm>
#include <functional>
#include <filesystem>

int main(int argc, char* argv[]) try {

    std::filesystem::path exe_path(argv[0]);
    std::string dir = exe_path.parent_path().string() + "/";
    std::string kernel_file = dir + "bitonic_sort.cl";

    std::vector<int> data;
    bitonic_sort::read_data(std::cin, std::back_inserter(data));

    std::vector<int> data4gpu, data4cpu, data4sort;
    data4gpu = data4cpu = data4sort = data;

    using vector_int_it = std::vector<int>::iterator;
    long double bitonic_sort_gpu_time = get_time(
        bitonic_sort::bitonic_sort_gpu<vector_int_it>, data4gpu.begin(), data4gpu.end(), kernel_file
    );

    long double bitonic_sort_cpu_time = get_time(
        bitonic_sort::bitonic_sort_cpu<vector_int_it>, data4cpu.begin(), data4cpu.end()
    );

    long double sort_time = get_time(
        [](auto&& begin, auto&& end) { std::sort(begin, end); }, data4sort.begin(), data4sort.end()
    );

    std::cout << data.size() << '\n';
    std::cout << bitonic_sort_gpu_time << '\n';
    std::cout << bitonic_sort_cpu_time << '\n';
    std::cout << sort_time << '\n';

    NO_OPT(data4gpu);
    NO_OPT(data4cpu);
    NO_OPT(data4sort);

} catch (const bitonic_sort::error_t& error) {
    std::cout << error.what() << '\n';
    return 1;
} catch (...) {
    std::cout << print_red("UNKNOWN ERROR\n");
    return 1;
}