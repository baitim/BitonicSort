#pragma once

#include "BitonicSort/cl/context.hpp"
#include <iostream>

namespace bitonic_sort {

    template <typename ElemT>
    concept sortable_elem_t = requires(ElemT elem) {
        {elem <  elem} -> std::convertible_to<bool>;
        {elem >  elem} -> std::convertible_to<bool>;
    };

    template <sortable_elem_t ElemT>
    void bitonic_sort(std::vector<ElemT> &data) {
        platform_t platform;
        context_t context(platform);
    }
}