#pragma once

#include "BitonicSort/cl/command_queue.hpp"

namespace bitonic_sort {
    class memory_t final : public detail::wrapper_t<cl_mem> {
        size_t size_ = 0;
        command_queue_t command_queue_;

    public:
        memory_t(const command_queue_t& command_queue, cl_mem_flags flags, size_t size)
        : size_(size), command_queue_(command_queue) {
            obj_ = cl_handler(clCreateBuffer, command_queue.context().obj(), flags, size, nullptr, nullptr);
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

    inline std::ostream& operator<<(std::ostream& os, memory_t& memory) {
        os << print_blue("memory\n");
        os << print_blue("\tCL_MEM_SIZE: ") <<
              print_lcyan(memory.get_info<CL_MEM_SIZE>() << "\n");
        os << print_blue("\tCL_MEM_HOST_PTR: ") <<
              print_lcyan(memory.get_info<CL_MEM_HOST_PTR>());
        return os;
    }
}