#include "BitonicSort/cl/bitonic_sort.hpp"
#include <filesystem>

int main(int argc, char* argv[]) {

    std::filesystem::path exe_path(argv[0]);
    std::string dir = exe_path.parent_path().string() + "/";
    try {
        bitonic_sort::platform_t platform;
        std::cout << platform.get_info<CL_PLATFORM_NAME>() << "\n";
        std::cout << platform.get_info<CL_PLATFORM_PROFILE>() << "\n";
        std::cout << platform.get_info<CL_PLATFORM_VERSION>() << "\n";
        std::cout << platform.get_info<CL_PLATFORM_VENDOR>() << "\n";

        bitonic_sort::context_t context{platform};
        std::cout << context.get_info<CL_CONTEXT_REFERENCE_COUNT>() << "\n";
        std::cout << context.get_info<CL_CONTEXT_NUM_DEVICES>() << "\n";

    } catch (cl::Error& error) {
        std::cout << error.what() << "\n";
        return 1;
    } catch (const bitonic_sort::error_t& error) {
        std::cout << error.what() << "\n";
        return 1;
    }

    return 0;
}