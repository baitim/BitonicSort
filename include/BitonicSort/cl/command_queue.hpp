#pragma once

#include "BitonicSort/cl/context.hpp"

namespace bitonic_sort {
    class command_queue_t final : public detail::wrapper_t<cl_command_queue> {
        context_t context_;

    public:
        command_queue_t(const context_t& context)
        : detail::wrapper_t<cl_command_queue>(
            cl_handler(clCreateCommandQueue, context.obj(), context.device().obj(), 0, nullptr)
        ), context_(context) {}

        command_queue_t(const command_queue_t& rhs)
        : detail::wrapper_t<cl_command_queue>(rhs), context_(rhs.context_) {}

        const context_t& context() const { return context_; }

        template <cl_command_queue_info command_queue_name>
        typename detail::param_traits<cl_command_queue_info, command_queue_name>::type 
        get_info() const {
            return wrapper_get_info<command_queue_name>();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, command_queue_t& command_queue) {
        os << print_blue("command queue");
        return os;
    }
}