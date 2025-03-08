#include "BitonicSort/cl/bitonic_sort.hpp"
#include "benchmark.hpp"
#include <algorithm>
#include <functional>
#include <filesystem>

int main(int argc, char* argv[]) {

    std::filesystem::path exe_path(argv[0]);
    std::string dir = exe_path.parent_path().string() + "/";
    std::string kernel_file = dir + "bitonic_sort.cl";
    try {

        std::vector<int> data;
        bitonic_sort::read_data(std::cin, data);

        std::vector<int> data4gpu, data4cpu, data4sort;
        data4gpu = data4cpu = data4sort = data;

        long double bitonic_sort_gpu_time = get_time(bitonic_sort::bitonic_sort_gpu<int>, data4gpu, kernel_file);
        long double bitonic_sort_cpu_time = get_time(bitonic_sort::bitonic_sort_cpu<int>, data4cpu);
        long double sort_time = get_time(
            [](auto&& begin, auto&& end) { std::sort(begin, end); }, data4sort.begin(), data4sort.end()
        );

        std::cout << data.size() << "\n";
        std::cout << bitonic_sort_gpu_time << "\n";
        std::cout << bitonic_sort_cpu_time << "\n";
        std::cout << sort_time << "\n";

        NO_OPT(data4gpu);
        NO_OPT(data4cpu);
        NO_OPT(data4sort);

    } catch (cl::Error& error) {
        std::cout << error.what() << "\n";
        return 1;
    } catch (const bitonic_sort::error_t& error) {
        std::cout << error.what() << "\n";
        return 1;
    }

    return 0;
}