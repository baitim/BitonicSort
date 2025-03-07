#include "BitonicSort/cl/bitonic_sort.hpp"
#include <gtest/gtest.h>
#include <filesystem>

std::string executable_path;

std::vector<std::string> get_sorted_files(std::filesystem::path path) {
    std::vector<std::string> files;

    for (const auto& entry : std::filesystem::directory_iterator(path))
        files.push_back(entry.path().string());

    std::sort(files.begin(), files.end());
    return files;
}

void get_numbers_from_file(std::vector<int>& numbers, std::string_view file_name) {
    std::ifstream file(std::string{file_name});
    int x;
    while (file >> x)
        numbers.push_back(x);
    file.close();
}

TEST(BitonicSort_shuffle, cmp_ete_with_core) {
    std::string file{__FILE__};
    std::filesystem::path dir = file.substr(0, file.rfind("/"));
    dir = dir / "../end_to_end";

    std::filesystem::path answers_path      = dir / "answers/";
    std::filesystem::path answers_core_path = dir / "answers_core/";

    std::vector<std::string> answers_str      = get_sorted_files(answers_path);
    std::vector<std::string> answers_core_str = get_sorted_files(answers_core_path);
    const unsigned count_tests = std::min(answers_str.size(), answers_core_str.size());

    for (unsigned i = 0; i < count_tests; ++i) {
        std::vector<int> answer, answer_core;
        get_numbers_from_file(answer, answers_str[i]);
        get_numbers_from_file(answer_core, answers_core_str[i]);

        ASSERT_EQ(answer.size(), answer_core.size()) << "in test : " << i + 1 << "\n";;
        for (int j = 0, end = answer.size(); j < end; ++j)
            EXPECT_EQ(answer[j], answer_core[j]) << "in test : " << i + 1 << " in number: " << j + 1 << "\n";;
    }
}

TEST(BitonicSort_shuffle, simple_test_gpu_1) {
    std::string dir = std::filesystem::path(executable_path).parent_path().string() + "/";
    std::string kernel_file = dir + "bitonic_sort.cl";

    std::vector<int> data_gpu = {6, 1, 4, 5, 7, 2, 3, 8};
    std::vector<int> data_cpu = data_gpu;

    bitonic_sort::bitonic_sort_gpu(data_gpu, kernel_file);
    bitonic_sort::bitonic_sort_cpu(data_cpu);

    std::vector<int> answer = {1, 2, 3, 4, 5, 6, 7, 8};
    ASSERT_EQ(data_gpu, answer);
    ASSERT_EQ(data_cpu, answer);
}

int main(int argc, char **argv) {
    executable_path = argv[0];
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}