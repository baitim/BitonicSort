#pragma once

#include "BitonicSort/cl/platform.hpp"

namespace bitonic_sort {
    class device_t final : public detail::wrapper_t<cl_device_id> {
        platform_t platform_;

    private:
        cl_device_id select_device() {
            cl_uint num_devices = 0;
            cl_handler(clGetDeviceIDs, platform_.obj(), CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices);

            if (num_devices == 0)
                throw error_t{str_red("No devices found on this platform")};

            std::vector<cl_device_id> devices(num_devices);
            cl_handler(clGetDeviceIDs, platform_.obj(), CL_DEVICE_TYPE_ALL, num_devices, devices.data(), nullptr);
            return devices[0];
        }

    public:
        device_t() { obj_ = select_device(); }

        device_t(const device_t& rhs)
        : detail::wrapper_t<cl_device_id>(rhs.obj_), platform_(rhs.platform_) {}

        device_t& operator=(const device_t& rhs) {
            if (this == &rhs)
                return *this;

            device_t new_device{rhs};
            std::swap(*this, new_device);
            return *this;
        }

        device_t(device_t&& rhs) noexcept : detail::wrapper_t<cl_device_id>(std::move(rhs.obj_)),
                                            platform_(std::move(rhs.platform_)) {}

        device_t& operator=(device_t&& rhs) noexcept {
            if (this == &rhs)
                return *this;

            detail::wrapper_t<cl_device_id>::operator=(std::move(rhs.obj_));
            std::swap(platform_, rhs.platform_);
            return *this;
        }

        template <cl_device_info device_name>
        typename detail::param_traits<cl_device_info, device_name>::type 
        get_info() const {
            return wrapper_get_info<device_name>();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const device_t& device) {
        os << print_blue("device\n");
        os << print_blue("\tCL_DEVICE_MAX_WORK_GROUP_SIZE: ") <<
              print_lcyan(device.get_info<CL_DEVICE_MAX_WORK_GROUP_SIZE>());
        return os;
    }
}