#pragma once

#include "BitonicSort/cl/command_queue.hpp"
#include <memory>

namespace bitonic_sort {
    class memory_t final : public detail::wrapper_t<cl_mem> {
        size_t size_ = 0;
        command_queue_t command_queue_;

    public:
        template <typename It>
        memory_t(const command_queue_t& command_queue, cl_mem_flags flags, It begin, It end)
        : command_queue_(command_queue) {

            using ElemT = typename std::iterator_traits<It>::value_type;
            size_ = sizeof(ElemT) * std::distance(begin, end);

            obj_ = cl_handler(clCreateBuffer, command_queue.context().obj(), flags, size_, nullptr, nullptr);

            std::unique_ptr<ElemT[]> data = std::make_unique<ElemT[]>(size_);
            std::copy(begin, end, data.get());

            cl_handler(
                clEnqueueWriteBuffer,
                    command_queue_.obj(), obj_, CL_TRUE, 0, size_, data.get(), 0, nullptr, nullptr
            );
        }

        template <typename It>
        void get(It begin, size_t size) const {
            using ElemT = typename std::iterator_traits<It>::value_type;

            size_t count_elems = std::min(size, size_ / sizeof(ElemT));
            std::unique_ptr<ElemT[]> data = std::make_unique<ElemT[]>(count_elems);

            size_t size_in_bytes = count_elems * sizeof(ElemT);
            cl_handler(
                clEnqueueReadBuffer,
                    command_queue_.obj(), obj_, CL_TRUE, 0, size_in_bytes, data.get(), 0, nullptr, nullptr
            );

            std::copy(data.get(), data.get() + count_elems, begin);
        }

        memory_t(const memory_t& rhs)
        : detail::wrapper_t<cl_mem>(rhs.obj_), size_(rhs.size_), command_queue_(rhs.command_queue_) {}

        memory_t& operator=(const memory_t& rhs) {
            if (this == &rhs)
                return *this;

            memory_t new_memory{rhs};
            std::swap(*this, new_memory);
            return *this;
        }

        memory_t(memory_t&& rhs) noexcept : detail::wrapper_t<cl_mem>(std::move(rhs.obj_)),
                                            size_         (std::move(rhs.size_)),
                                            command_queue_(std::move(rhs.command_queue_)) {}

        memory_t& operator=(memory_t&& rhs) noexcept {
            if (this == &rhs)
                return *this;

            detail::wrapper_t<cl_mem>::operator=(std::move(rhs.obj_));
            std::swap(size_,          rhs.size_);
            std::swap(command_queue_, rhs.command_queue_);
            return *this;
        }

        template <cl_command_queue_info memory_name>
        typename detail::param_traits<cl_command_queue_info, memory_name>::type 
        get_info() const {
            return wrapper_get_info<memory_name>();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const memory_t& memory) {
        os << print_blue("memory\n");
        os << print_blue("\tCL_MEM_SIZE: ") <<
              print_lcyan(memory.get_info<CL_MEM_SIZE>() << '\n');
        os << print_blue("\tCL_MEM_HOST_PTR: ") <<
              print_lcyan(memory.get_info<CL_MEM_HOST_PTR>());
        return os;
    }
}