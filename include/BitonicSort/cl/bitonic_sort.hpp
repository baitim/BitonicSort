#pragma once

#include "BitonicSort/cl/kernel.hpp"
#include <math.h>
#include <iostream>

namespace bitonic_sort {

    template <typename ElemT>
    concept sortable_elem_t =
        requires(ElemT elem) {
            { elem > elem } -> std::convertible_to<bool>;
            { std::cin >> elem } -> std::same_as<std::istream&>;
            { std::numeric_limits<ElemT>::max() } -> std::convertible_to<ElemT>;
        } && std::copyable<ElemT>
          && std::default_initializable<ElemT>;

    template <typename ContainerT>
    concept sortable_container_t =
        std::ranges::random_access_range<ContainerT> &&
        sortable_elem_t<typename ContainerT::value_type>;

    template <typename ContainerT>
    requires sortable_elem_t<typename ContainerT::value_type>
    inline void read_data(std::istream& is, std::back_insert_iterator<ContainerT> out) {
        using ElemT = typename ContainerT::value_type;

        int size;
        is >> size;
        if (!is.good())
            throw error_t{str_red("Invalid count of elements")};
        if (size <= 0)
            throw error_t{str_red("Count of elements <= 0")};

        for (int i = 0; i < size; ++i) {
            ElemT elem;
            is >> elem;
            *out++ = elem;
        }
    }

    template <sortable_container_t ContainerT, sortable_elem_t ElemT>
    inline void resize2pow2(ContainerT& data, const ElemT& filling_elem) {
        int new_size = std::bit_ceil(data.size());
        data.resize(new_size, filling_elem);
    }

    template <sortable_container_t ContainerT>
    inline void bitonic_merge_cpu(ContainerT& data, size_t low, size_t size, bool ascending) {
        using ElemT = typename ContainerT::value_type;

        for (size_t block = size / 2; block > 0; block /= 2) {
            size_t count_blocks = size / 2 / block;

            for (size_t i = 0; i < count_blocks; ++i) {
                size_t block_shift = low + 2 * block * i;
                size_t next_block_shift = block_shift + block;

                for (size_t j = 0; j < block; ++j) {
                    ElemT& lhs = data[block_shift      + j];
                    ElemT& rhs = data[next_block_shift + j];

                    if ((lhs > rhs) == ascending)
                        std::swap(lhs, rhs);
                }
            }
        }
    }

    template <sortable_container_t ContainerT>
    inline void bitonic_split_cpu(ContainerT& data, size_t size) {
        for (size_t n = 2; n <= size; n *= 2) {
            for (size_t low = 0, i = 1; low < size; low += n, ++i) {
                bitonic_merge_cpu(data, low, n, i % 2);
            }
        }
    }

    template <sortable_container_t ContainerT>
    inline void bitonic_sort_cpu(ContainerT& data) {
        using ElemT = typename ContainerT::value_type;

        const size_t old_size = data.size();
        if (old_size <= 1) return;

        const ElemT max_elem = std::numeric_limits<ElemT>::max();
        resize2pow2(data, max_elem);

        bitonic_split_cpu(data, data.size());
        data.resize(old_size);
    }

    template <sortable_container_t ContainerT>
    inline void bitonic_sort_gpu(ContainerT& data, std::string_view kernel_file) {
        using ElemT = typename ContainerT::value_type;

        const size_t old_size = data.size();
        if (old_size <= 1) return;

        const ElemT max_elem = std::numeric_limits<ElemT>::max();
        resize2pow2(data, max_elem);
        const size_t size = data.size();

        bitonic_sort::device_t        device;
        bitonic_sort::context_t       context{device};
        bitonic_sort::command_queue_t command_queue{context};
        bitonic_sort::program_t       program(context, kernel_file);
        bitonic_sort::kernel_t        kernel(program, command_queue, "bitonic_merge");
        bitonic_sort::memory_t        memory(command_queue, CL_MEM_READ_WRITE, data.begin(), data.end());

        size_t max_work_group_size = device.get_info<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
        size_t local_size = std::min(size, max_work_group_size);

        kernel.set_memory(0, memory);
        kernel.set_local_memory(1, sizeof(ElemT) * local_size);

        for (size_t step = 2; step <= size; step *= 2) {
            kernel.set_argument(2, static_cast<cl_int>(step));
            for (size_t stage = step / 2; stage > 0; stage /= 2) {
                kernel.set_argument(3, static_cast<cl_int>(stage));
                kernel.execute(size, local_size);
                command_queue.barrier();
            }
        }

        memory.get(data.begin(), old_size);
        data.resize(old_size);
    }
}