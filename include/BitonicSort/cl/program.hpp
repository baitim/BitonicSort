#pragma once

#include "BitonicSort/cl/context.hpp"
#include <memory>

namespace bitonic_sort {
    class program_t final : public detail::wrapper_t<cl_program> {
        context_t context_;

    public:
        program_t(const context_t& context, std::string_view file_name) : context_(context) {
            std::string kernel_code = file2str(file_name);
            const char *data = kernel_code.data();
            size_t size = kernel_code.size();

            obj_ = cl_handler(clCreateProgramWithSource, context_.obj(), 1, &data, &size, nullptr);

            cl_int err = clBuildProgram(obj_, 1, &context_.device()(), nullptr, nullptr, nullptr);
            if (err == CL_SUCCESS)
                return;

            size_t log_size;
            clGetProgramBuildInfo(obj_, context_.device()(), CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);

            std::unique_ptr<char[]> log(new char[log_size]);
            cl_handler(
                clGetProgramBuildInfo,
                    obj_, context_.device()(), CL_PROGRAM_BUILD_LOG, log_size, log.get(), nullptr
            );

            throw error_t{str_red(std::string{"Error in OpenCL program build: "} + log.get())};
        }

        program_t(const program_t& rhs)
        : detail::wrapper_t<cl_program>(rhs.obj_), context_(rhs.context_) {}

        program_t& operator=(const program_t& rhs) {
            if (this == &rhs)
                return *this;

            program_t new_program{rhs};
            std::swap(*this, new_program);
            return *this;
        }

        program_t(program_t&& rhs) noexcept : detail::wrapper_t<cl_program>(std::move(rhs.obj_)),
                                              context_(std::move(rhs.context_)) {}

        program_t& operator=(program_t&& rhs) noexcept {
            if (this == &rhs)
                return *this;

            detail::wrapper_t<cl_program>::operator=(std::move(rhs.obj_));
            std::swap(context_, rhs.context_);
            return *this;
        }

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