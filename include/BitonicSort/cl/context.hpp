#pragma once

#include "BitonicSort/cl/platform.hpp"

namespace bitonic_sort {
    class context_t final : public detail::wrapper_t<cl_context> {
    private:
        static cl_context get_gpu_context(cl_platform_id platform) {
            cl_context_properties properties[] = {
                CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform), 0
            };

            cl_context context = cl_handler(
                clCreateContextFromType, properties, CL_DEVICE_TYPE_GPU, nullptr, nullptr, nullptr
            );

            return context;
        }

    public:
        context_t(platform_t platform) : detail::wrapper_t<cl_context>(get_gpu_context(platform.obj())) {}

        context_t(const context_t& context, bool is_retain = false)
        : detail::wrapper_t<cl_context>(context.obj(), is_retain) {}

        template <cl_context_info context_name>
        typename detail::param_traits<cl_context_info, context_name>::type 
        get_info() const {
            return wrapper_get_info<cl_context_info, context_name>();
        }
    };
}