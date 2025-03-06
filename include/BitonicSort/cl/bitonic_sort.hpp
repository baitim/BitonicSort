#pragma once

#include "BitonicSort/cl/kernel.hpp"
#include "BitonicSort/cl/memory.hpp"
#include <iostream>

namespace bitonic_sort {

    template <typename ElemT>
    concept sortable_elem_t = requires(ElemT elem) {
        {elem < elem} -> std::convertible_to<bool>;
        {elem > elem} -> std::convertible_to<bool>;
        {std::cin >> elem} -> std::same_as<std::istream&>;
    };

    template <sortable_elem_t ElemT>
    inline void read_data(std::istream& is, std::vector<ElemT>& data) {
        int size;
        is >> size;
        if (!is.good())
            throw error_t{str_red("Invalid count of elements")};
        if (size <= 0)
            throw error_t{str_red("Count of elements <= 0")};

        data.resize(size);
        for (int i = 0; i < size; ++i)
            is >> data[i];
    }

    template <sortable_elem_t ElemT>
    void resize2pow2(std::vector<ElemT>& data, const ElemT& filling_elem) {
        int n = data.size();
        int new_size = 1;
        while (new_size < n)
            new_size *= 2;
        data.resize(new_size, filling_elem);
    }

    template <sortable_elem_t ElemT>
    void bitonic_merge_cpu(std::vector<ElemT>& data, size_t low, size_t size, bool ascending) {
        for (size_t k = size / 2; k > 0; k /= 2) {
            int shift = size - k;
            for (size_t i = low; i < low + shift; i++) {
                if ((data[i] > data[i + k]) == ascending)
                    std::swap(data[i], data[i + k]);
            }
        }
    }

    template <sortable_elem_t ElemT>
    void bitonic_split_cpu(std::vector<ElemT>& data, size_t size) {
        for (size_t n = 2; n <= size; n *= 2) {
            for (size_t low = 0, i = 1; low < size; low += n, ++i) {
                bool ascending = i % 2;
                bitonic_merge_cpu(data, low, n, ascending);
            }
        }
    }

    template <sortable_elem_t ElemT>
    inline void bitonic_sort_cpu(std::vector<ElemT>& data) {
        ElemT max_elem = std::numeric_limits<ElemT>::max();
        resize2pow2(data, max_elem);
        
        size_t size = data.size();
        bitonic_split_cpu(data, size);

        data.erase(std::remove(data.begin(), data.end(), max_elem), data.end());
    }

    template <sortable_elem_t ElemT>
    inline void bitonic_sort_gpu(std::vector<ElemT>& data, std::string_view kernel_file) {
        bitonic_sort::device_t        device;
        bitonic_sort::context_t       context{device};
        bitonic_sort::command_queue_t command_queue{context};
        bitonic_sort::program_t       program(context, kernel_file);
        bitonic_sort::kernel_t        kernel(program, command_queue, "vector_add");

        size_t memory_size = data.size() * sizeof(ElemT);
        bitonic_sort::memory_t memory(command_queue, CL_MEM_READ_WRITE, memory_size);
    }
}