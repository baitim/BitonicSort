#pragma once

#include "BitonicSort/cl/cl.hpp"

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
                cl_int error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &num_devices);
                if (error == CL_DEVICE_NOT_FOUND)
                    cl_handler(clGetDeviceIDs, platform, CL_DEVICE_TYPE_CPU, 0, nullptr, &num_devices);
                else
                    check_cl_error(error, clGetDeviceIDs);
                
                if (num_devices > 0)
                    return platform;
            }

            throw error_t{str_red("No platform with GPU devices found")};
        }

    public:
        platform_t() : detail::wrapper_t<cl_platform_id>(select_platform()) {}

        platform_t(const platform_t& rhs)
        : detail::wrapper_t<cl_platform_id>(rhs.obj_) {}

        platform_t& operator=(const platform_t& rhs) {
            if (this == &rhs)
                return *this;

            platform_t new_platform{rhs};
            std::swap(*this, new_platform);
            return *this;
        }

        platform_t(platform_t&& rhs) noexcept : detail::wrapper_t<cl_platform_id>(std::move(rhs.obj_)) {}

        platform_t& operator=(platform_t&& rhs) noexcept {
            if (this == &rhs)
                return *this;

            detail::wrapper_t<cl_platform_id>::operator=(std::move(rhs.obj_));
            return *this;
        }

        template <cl_platform_info platform_name>
        typename detail::param_traits<cl_platform_info, platform_name>::type 
        get_info() const {
            return wrapper_get_info<platform_name>();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const platform_t& platform) {
        os << print_blue("platform\n");
        os << print_blue("\tCL_PLATFORM_NAME: ") <<
              print_lcyan(platform.get_info<CL_PLATFORM_NAME>() << '\n');
        os << print_blue("\tCL_PLATFORM_PROFILE: ") <<
              print_lcyan(platform.get_info<CL_PLATFORM_PROFILE>() << '\n');
        os << print_blue("\tCL_PLATFORM_VERSION: ") <<
              print_lcyan(platform.get_info<CL_PLATFORM_VERSION>() << '\n');
        os << print_blue("\tCL_PLATFORM_VENDOR: ") <<
              print_lcyan(platform.get_info<CL_PLATFORM_VENDOR>());
        return os;
    }
}