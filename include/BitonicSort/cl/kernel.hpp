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

        kernel_t(const kernel_t& kernel)
        : detail::wrapper_t<cl_kernel>(kernel.obj_), program_(kernel.program_), queue_(kernel.queue_) {}

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