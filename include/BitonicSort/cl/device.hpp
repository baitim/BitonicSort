#pragma once

#include "BitonicSort/cl/platform.hpp"

namespace bitonic_sort {
    class device_t final : public detail::wrapper_t<cl_device_id> {
        platform_t platform_;

    public:
        device_t() {
            cl_handler(clGetDeviceIDs, platform_.obj(), CL_DEVICE_TYPE_DEFAULT, 1, &obj_, nullptr);
        }

        device_t(const device_t& device)
        : detail::wrapper_t<cl_device_id>(device.obj_) {}

        template <cl_device_info device_name>
        typename detail::param_traits<cl_device_info, device_name>::type 
        get_info() const {
            return wrapper_get_info<device_name>();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, device_t& device) {
        os << print_blue("device\n");
        os << print_blue("\tCL_DEVICE_MAX_WORK_GROUP_SIZE: ") <<
              print_lcyan(device.get_info<CL_DEVICE_MAX_WORK_GROUP_SIZE>());
        return os;
    }
}