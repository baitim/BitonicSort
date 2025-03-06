#pragma once

#include "BitonicSort/cl/command_queue.hpp"
#include "BitonicSort/cl/program.hpp"

namespace bitonic_sort {
    class kernel_t final : public detail::wrapper_t<cl_kernel> {
        program_t program_;
        command_queue_t queue_;

    public:
        kernel_t(const program_t& program, const command_queue_t& queue, std::string_view func_name)
        : program_(program), queue_(queue) {
            obj_ = cl_handler(clCreateKernel, program_.obj(), func_name.data(), nullptr);
        }

        kernel_t(const kernel_t& rhs)
        : detail::wrapper_t<cl_kernel>(rhs.obj_), program_(rhs.program_), queue_(rhs.queue_) {}

        kernel_t& operator=(const kernel_t& rhs) {
            if (this == &rhs)
                return *this;

            kernel_t new_kernel{rhs};
            std::swap(*this, new_kernel);
            return *this;
        }

        kernel_t(kernel_t&& rhs) noexcept : detail::wrapper_t<cl_kernel>(std::move(rhs.obj_)),
                                            program_(std::move(rhs.program_)),
                                            queue_  (std::move(rhs.queue_)) {}

        kernel_t& operator=(kernel_t&& rhs) noexcept {
            if (this == &rhs)
                return *this;

            detail::wrapper_t<cl_kernel>::operator=(std::move(rhs.obj_));
            std::swap(program_, rhs.program_);
            std::swap(queue_,   rhs.queue_);
            return *this;
        }

        template <cl_kernel_info kernel_name>
        typename detail::param_traits<cl_kernel_info, kernel_name>::type 
        get_info() const {
            return wrapper_get_info<kernel_name>();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, kernel_t& kernel) {
        os << print_blue("kernel\n");
        os << print_blue("\tCL_KERNEL_FUNCTION_NAME: ") <<
              print_lcyan(kernel.get_info<CL_KERNEL_FUNCTION_NAME>() << "\n");
        os << print_blue("\tCL_KERNEL_ATTRIBUTES: ") <<
              print_lcyan(kernel.get_info<CL_KERNEL_ATTRIBUTES>());
        return os;
    }
}