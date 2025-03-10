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
        : detail::wrapper_t<cl_command_queue>(rhs.obj_), context_(rhs.context_) {}

        command_queue_t& operator=(const command_queue_t& rhs) {
            if (this == &rhs)
                return *this;

            command_queue_t new_command_queue{rhs};
            std::swap(*this, new_command_queue);
            return *this;
        }

        command_queue_t(command_queue_t&& rhs) noexcept : detail::wrapper_t<cl_command_queue>(std::move(rhs.obj_)),
                                                          context_(std::move(rhs.context_)) {}

        command_queue_t& operator=(command_queue_t&& rhs) noexcept {
            if (this == &rhs)
                return *this;

            detail::wrapper_t<cl_command_queue>::operator=(std::move(rhs.obj_));
            std::swap(context_, rhs.context_);
            return *this;
        }

        void barrier() {
            cl_handler(clFinish, obj_);
        }

        const context_t& context() const { return context_; }

        template <cl_command_queue_info command_queue_name>
        typename detail::param_traits<cl_command_queue_info, command_queue_name>::type 
        get_info() const {
            return wrapper_get_info<command_queue_name>();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const command_queue_t& command_queue) {
        os << print_blue("command queue");
        return os;
    }
}