#include "BitonicSort/cl/bitonic_sort.hpp"
#include <filesystem>

int main(int argc, char* argv[]) {

    std::filesystem::path exe_path(argv[0]);
    std::string dir = exe_path.parent_path().string() + "/";
    try {
        bitonic_sort::device_t        device;
        bitonic_sort::context_t       context{device};
        bitonic_sort::command_queue_t command_queue{context};

    } catch (cl::Error& error) {
        std::cout << error.what() << "\n";
        return 1;
    } catch (const bitonic_sort::error_t& error) {
        std::cout << error.what() << "\n";
        return 1;
    }

    return 0;
}