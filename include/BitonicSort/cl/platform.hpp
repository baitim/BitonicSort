#pragma once

#include "BitonicSort/cl/cl.hpp"
#include <iostream>

namespace bitonic_sort {
    class platform_t final : public detail::wrapper_t<cl_platform_id> {
    private:
        cl_platform_id select_platform() {
            cl_uint num_platforms = 0;
            cl_handler(clGetPlatformIDs, 0, nullptr, &num_platforms);
            if (num_platforms == 0)
                throw error_t{str_red("No OpenCL platforms found")};

            std::vector<cl_platform_id> platforms(num_platforms);
            cl_handler(clGetPlatformIDs, num_platforms, platforms.data(), nullptr);

            cl_uint num_devices;
            for (auto platform : platforms) {
                cl_handler(clGetDeviceIDs, platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &num_devices);
                if (num_devices > 0)
                    return platform;
            }

            throw error_t{str_red("No platform with GPU devices found")};
        }

    public:
        platform_t() : detail::wrapper_t<cl_platform_id>(select_platform()) {}

        platform_t(const platform_t& platform, bool is_retain = false)
        : detail::wrapper_t<cl_platform_id>(platform.obj(), is_retain) {}

        template <cl_platform_info platform_name>
        typename detail::param_traits<cl_platform_info, platform_name>::type 
        get_info() const {
            return wrapper_get_info<cl_platform_info, platform_name>();
        }
    };
}