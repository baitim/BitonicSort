#pragma once

#include "BitonicSort/cl/kernel.hpp"
#include <iostream>

namespace bitonic_sort {

    template <typename ElemT>
    concept sortable_elem_t = requires(ElemT elem) {
        {elem < elem} -> std::convertible_to<bool>;
        {elem > elem} -> std::convertible_to<bool>;
    };

    template <sortable_elem_t ElemT>
    void bitonic_sort(std::vector<ElemT> &data) {
        bitonic_sort::device_t        device;
        bitonic_sort::context_t       context{device};
        bitonic_sort::command_queue_t command_queue{context};
    }
}