#pragma once

#include "BitonicSort/cl/context.hpp"

namespace bitonic_sort {
    class program_t final : public detail::wrapper_t<cl_program> {
        context_t context_;

    public:
        program_t(const context_t& context, std::string_view file_name) : context_(context) {
            std::string kernel_code = file2str(file_name);
            const char *data = kernel_code.data();
            size_t size = kernel_code.size();

            obj_ = cl_handler(clCreateProgramWithSource, context_.obj(), 1, &data, &size, nullptr);
            cl_handler(clBuildProgram, obj_, 1, &context_.device()(), nullptr, nullptr, nullptr);
        }

        program_t(const program_t& program)
        : detail::wrapper_t<cl_program>(program.obj_), context_(program.context_) {}

        const context_t& context() const { return context_; }

        template <cl_program_info program_name>
        typename detail::param_traits<cl_program_info, program_name>::type 
        get_info() const {
            return wrapper_get_info<program_name>();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, program_t& program) {
        os << print_blue("program\n");
        os << print_blue("\tCL_PROGRAM_SOURCE: ") <<
              print_lcyan(program.get_info<CL_PROGRAM_SOURCE>() << "\n");
        os << print_blue("\tCL_PROGRAM_KERNEL_NAMES: ") <<
              print_lcyan(program.get_info<CL_PROGRAM_KERNEL_NAMES>());
        return os;
    }
}