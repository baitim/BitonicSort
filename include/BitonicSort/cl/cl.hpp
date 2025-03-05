#pragma once

#include "BitonicSort/common.hpp"

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"
#include <algorithm>
#include <vector>

namespace bitonic_sort {
    inline void check_cl_error(cl_int error, std::string_view func_name,
                               const char* file, const char* caller_func) {
        if (error != CL_SUCCESS) {
            std::stringstream ss;
            ss << "OpenCL error in: " << func_name << "\n"
               << "error code: " << error << "\n"
               << "called from: " << caller_func << " in " << file;

            throw error_t{str_red(ss.str())};
        }
    }
    
    #define cl_handler(func, ...) cl_handler_(func, #func, __FILE__, __func__ __VA_OPT__(,) __VA_ARGS__)
    
    template <typename Func, typename... Args>
    inline auto cl_handler_(Func func, std::string_view func_name, const char* file,
                            const char* caller_func, Args&&... args)
                            -> std::invoke_result_t<Func, Args...> {
        if constexpr(std::is_same_v<std::invoke_result_t<Func, Args...>, cl_int>) {
            cl_int error = std::invoke(func, std::forward<Args>(args)...);
            check_cl_error(error, func_name, file, caller_func);
            return error;
        } else {
            auto result = std::invoke(func, std::forward<Args>(args)...);
            return result;
        }
    }

    template <typename Func, typename... Args>
    inline auto cl_handler_(Func func, std::string_view func_name, const char* file,
                            const char* caller_func, cl_int* error, Args&&... args)
                            -> std::invoke_result_t<Func, Args...> {
        cl_int local_error;
        cl_int* error_ = (error != nullptr) ? error : &local_error;
        auto result = std::invoke(func, std::forward<Args>(args)..., error_);
        check_cl_error(local_error, func_name, file, caller_func);
        return result;
    }
}


namespace bitonic_sort::detail {

    template <typename T> struct ReferenceHandler {};

    template <> struct ReferenceHandler<cl_platform_id> {
        static cl_int retain (cl_platform_id platform) { return 0; }
        static cl_int release(cl_platform_id platform) { return 0; }
    };

    template <> struct ReferenceHandler<cl_context> {
        static cl_int retain (cl_context context) { return ::clRetainContext (context); }
        static cl_int release(cl_context context) { return ::clReleaseContext(context); }
    };

    template <> struct ReferenceHandler<cl_program> {
        static cl_int retain (cl_program program) { return ::clRetainProgram (program); }
        static cl_int release(cl_program program) { return ::clReleaseProgram(program); }
    };

    template <> struct ReferenceHandler<cl_device_id> {
        static cl_int retain (cl_device_id device) { return ::clRetainDevice (device); }
        static cl_int release(cl_device_id device) { return ::clReleaseDevice(device); }
    };

    template <> struct ReferenceHandler<cl_command_queue> {
        static cl_int retain (cl_command_queue command_queue) { return ::clRetainCommandQueue (command_queue); }
        static cl_int release(cl_command_queue command_queue) { return ::clReleaseCommandQueue(command_queue); }
    };

    template <> struct ReferenceHandler<cl_kernel> {
        static cl_int retain (cl_kernel kernel) { return ::clRetainKernel (kernel); }
        static cl_int release(cl_kernel kernel) { return ::clReleaseKernel(kernel); }
    };

    template <> struct ReferenceHandler<cl_mem> {
        static cl_int retain (cl_mem memory) { return ::clRetainMemObject (memory); }
        static cl_int release(cl_mem memory) { return ::clReleaseMemObject(memory); }
    };

    /*------------------------------------------------------------------------------------------*/

    struct platform_type {
        static constexpr auto get_info_func = &clGetPlatformInfo;;
    };

    struct context_type {
        static constexpr auto get_info_func = &clGetContextInfo;;
    };

    template <typename T, cl_int NameT> struct param_traits {};

    /*------------------------------------------------------------------------------------------*/

    template<> struct param_traits<cl_platform_info, CL_PLATFORM_NAME> : public platform_type {
        enum { value = CL_PLATFORM_NAME };
        using type = std::string;
        static type to_type(std::string &str) { return str; }
    };
    
    template<> struct param_traits<cl_platform_info, CL_PLATFORM_PROFILE> : public platform_type {
        enum { value = CL_PLATFORM_PROFILE };
        using type = std::string;
        static type to_type(std::string &str) { return str; }
    };
    
    template<> struct param_traits<cl_platform_info, CL_PLATFORM_VERSION> : public platform_type {
        enum { value = CL_PLATFORM_VERSION };
        using type = std::string;
        static type to_type(std::string &str) { return str; }
    };
    
    template<> struct param_traits<cl_platform_info, CL_PLATFORM_VENDOR> : public platform_type {
        enum { value = CL_PLATFORM_VENDOR };
        using type = std::string;
        static type to_type(std::string &str) { return str; }
    };
    
    template<> struct param_traits<cl_platform_info, CL_PLATFORM_EXTENSIONS> : public platform_type {
        enum { value = CL_PLATFORM_EXTENSIONS };
        using type = std::string;
        static type to_type(std::string &str) { return str; }
    };

    /*------------------------------------------------------------------------------------------*/

    template<> struct param_traits<cl_context_info, CL_CONTEXT_REFERENCE_COUNT> : public context_type {
        enum { value = CL_CONTEXT_REFERENCE_COUNT };
        using type = unsigned int;

        static type to_type(std::string &str) {
            type result;
            std::copy_n(str.data(), sizeof(type), reinterpret_cast<char*>(&result));
            return result;
        }
    };

    template<> struct param_traits<cl_context_info, CL_CONTEXT_NUM_DEVICES> : public context_type {
        enum { value = CL_CONTEXT_NUM_DEVICES };
        using type = unsigned int;

        static type to_type(std::string &str) {
            type result;
            std::copy_n(str.data(), sizeof(type), reinterpret_cast<char*>(&result));
            return result;
        }
    };

    template<> struct param_traits<cl_context_info, CL_CONTEXT_DEVICES> : public context_type {
        enum { value = CL_CONTEXT_DEVICES };
        using type = std::vector<cl_device_id>;

        static type to_type(std::string &str) {
            type devices;
            devices.resize(str.size() / sizeof(cl_device_id));
            std::copy_n(
                reinterpret_cast<const cl_device_id*>(str.data()),
                devices.size(),
                devices.data()
            );
            return devices;
        }
    };

    template<> struct param_traits<cl_context_info, CL_CONTEXT_PROPERTIES> : public context_type {
        enum { value = CL_CONTEXT_PROPERTIES };
        using type = std::vector<cl_context_properties>;

        static type to_type(std::string &str) {
            type properties;
            properties.resize(str.size() / sizeof(cl_context_properties));
            std::copy_n(
                reinterpret_cast<const cl_context_properties*>(str.data()),
                properties.size(),
                properties.data()
            );
            return properties;
        }
    };

    /*------------------------------------------------------------------------------------------*/

    template <typename ObjT>
    class wrapper_t {
    protected:
        ObjT obj_;

    public:
        wrapper_t(ObjT obj = NULL) : obj_(obj) {
            retain();
        }

        wrapper_t(const wrapper_t &rhs, bool is_retain = false) {
            if (this == &rhs)
                return;

            release();
            obj_ = rhs.obj_;

            if (is_retain)
                retain();
        }

        wrapper_t &operator=(const wrapper_t &rhs) {
            if (this == &rhs)
                return *this;

            wrapper_t new_obj{rhs};
            std::swap(*this, new_obj);
            return *this;
        }

        wrapper_t(wrapper_t &&rhs) noexcept : obj_(std::exchange(rhs.obj_, NULL)) {}

        wrapper_t &operator=(wrapper_t &&rhs) noexcept {
            obj_ = std::exchange(rhs.obj_, NULL);
            return *this;
        }

              ObjT& operator()()       { return obj_; }
        const ObjT& operator()() const { return obj_; }

        ObjT obj() const { return obj_; }

        cl_int retain()  const { return ReferenceHandler<ObjT>::retain (obj_); }
        cl_int release() const { return ReferenceHandler<ObjT>::release(obj_); }

        template <typename InfoT, InfoT NameT>
        typename detail::param_traits<InfoT, NameT>::type wrapper_get_info() const {
            using traits_type = detail::param_traits<InfoT, NameT>;
 
            auto get_info_func = traits_type::get_info_func;

            size_t size = 0;
            cl_handler(get_info_func, obj_, NameT, 0, nullptr, &size);

            std::string info(size, '\0');
            cl_handler(get_info_func, obj_, NameT, size, info.data(), nullptr);

            return traits_type::to_type(info);
        }

        virtual ~wrapper_t() { release(); }
    };
}