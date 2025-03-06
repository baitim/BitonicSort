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
    inline void bitonic_sort(std::vector<ElemT> &data, std::string_view kernel_file) {
        bitonic_sort::device_t        device;
        bitonic_sort::context_t       context{device};
        bitonic_sort::command_queue_t command_queue{context};
        bitonic_sort::program_t       program(context, kernel_file);
        bitonic_sort::kernel_t        kernel(program, command_queue, "vector_add");

        size_t memory_size = data.size() * sizeof(ElemT);
        bitonic_sort::memory_t        memory(command_queue, CL_MEM_READ_WRITE, memory_size);
    }
}