#pragma once

#include "BitonicSort/cl/device.hpp"

namespace bitonic_sort {
    class context_t final : public detail::wrapper_t<cl_context> {
        device_t device_;

    private:
        static cl_context get_gpu_context(cl_device_id device) {
            cl_platform_id platform;
            cl_handler(clGetDeviceInfo, device, CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &platform, nullptr);
        
            cl_context_properties properties[] = {
                CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform), 0
            };
        
            return cl_handler(
                clCreateContext, properties, 1, &device, nullptr, nullptr, nullptr
            );
        }

    public:
        context_t(const device_t& device)
        : detail::wrapper_t<cl_context>(get_gpu_context(device.obj())), device_(device) {}

        context_t(const context_t& context)
        : detail::wrapper_t<cl_context>(context.obj_), device_(context.device()) {}

        const device_t& device() const { return device_; }

        template <cl_context_info context_name>
        typename detail::param_traits<cl_context_info, context_name>::type 
        get_info() const {
            return wrapper_get_info<context_name>();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, context_t& context) {
        os << print_blue("context\n");
        os << print_blue("\tCL_CONTEXT_REFERENCE_COUNT: ") <<
              print_lcyan(context.get_info<CL_CONTEXT_REFERENCE_COUNT>() << "\n");
        os << print_blue("\tCL_CONTEXT_NUM_DEVICES: ") <<
              print_lcyan(context.get_info<CL_CONTEXT_NUM_DEVICES>());
        return os;
    }
}