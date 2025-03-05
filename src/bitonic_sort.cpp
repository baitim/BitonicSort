#include "BitonicSort/cl/bitonic_sort.hpp"
#include <filesystem>

int main(int argc, char* argv[]) {

    std::filesystem::path exe_path(argv[0]);
    std::string dir = exe_path.parent_path().string() + "/";
    std::string kernel_file = dir + "bitonic_sort.cl";
    try {
        bitonic_sort::device_t        device;
        bitonic_sort::context_t       context{device};
        bitonic_sort::command_queue_t command_queue{context};
        bitonic_sort::program_t       program(context, kernel_file);
        bitonic_sort::kernel_t        kernel(program, command_queue, "");

    } catch (cl::Error& error) {
        std::cout << error.what() << "\n";
        return 1;
    } catch (const bitonic_sort::error_t& error) {
        std::cout << error.what() << "\n";
        return 1;
    }

    return 0;
}