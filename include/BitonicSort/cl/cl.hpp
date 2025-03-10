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

    #define check_cl_error(error, func, ...) check_cl_error_(error, #func, __FILE__, __func__);
    
    inline void check_cl_error_(cl_int error, std::string_view func_name,
                               const char* file, const char* caller_func) {
        if (error != CL_SUCCESS) {
            std::stringstream ss;
            ss << "OpenCL error in: " << func_name << '\n'
               << "error code: " << error << '\n'
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
            check_cl_error_(error, func_name, file, caller_func);
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

    template <> struct ReferenceHandler<cl_device_id> {
        static cl_int retain (cl_device_id device) { return ::clRetainDevice (device); }
        static cl_int release(cl_device_id device) { return ::clReleaseDevice(device); }
    };

    template <> struct ReferenceHandler<cl_context> {
        static cl_int retain (cl_context context) { return ::clRetainContext (context); }
        static cl_int release(cl_context context) { return ::clReleaseContext(context); }
    };

    template <> struct ReferenceHandler<cl_command_queue> {
        static cl_int retain (cl_command_queue command_queue) { return ::clRetainCommandQueue (command_queue); }
        static cl_int release(cl_command_queue command_queue) { return ::clReleaseCommandQueue(command_queue); }
    };

    template <> struct ReferenceHandler<cl_program> {
        static cl_int retain (cl_program program) { return ::clRetainProgram (program); }
        static cl_int release(cl_program program) { return ::clReleaseProgram(program); }
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
        static constexpr auto get_info_func = &clGetPlatformInfo;
    };

    struct device_type {
        static constexpr auto get_info_func = &clGetDeviceInfo;
    };

    struct context_type {
        static constexpr auto get_info_func = &clGetContextInfo;
    };

    struct command_queue_type {
        static constexpr auto get_info_func = &clGetCommandQueueInfo;
    };

    struct program_type {
        static constexpr auto get_info_func = &clGetProgramInfo;
    };

    struct kernel_type {
        static constexpr auto get_info_func = &clGetKernelInfo;
    };

    struct memory_type {
        static constexpr auto get_info_func = &clGetMemObjectInfo;
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

     template<> struct param_traits<cl_device_info, CL_DEVICE_TYPE> : public device_type {
        enum { value = CL_DEVICE_TYPE };
        using type = cl_device_type;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };

    template<> struct param_traits<cl_device_info, CL_DEVICE_VENDOR_ID> : public device_type {
        enum { value = CL_DEVICE_VENDOR_ID };
        using type = cl_uint;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };

    template<> struct param_traits<cl_device_info, CL_DEVICE_MAX_COMPUTE_UNITS> : public device_type {
        enum { value = CL_DEVICE_MAX_COMPUTE_UNITS };
        using type = cl_uint;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };

    template<> struct param_traits<cl_device_info, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS> : public device_type {
        enum { value = CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS };
        using type = cl_uint;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };

    template<> struct param_traits<cl_device_info, CL_DEVICE_MAX_WORK_ITEM_SIZES> : public device_type {
        enum { value = CL_DEVICE_MAX_WORK_ITEM_SIZES };
        using type = std::vector<size_t>;
        static type to_type(std::string &str) {
            type result(str.size() / sizeof(size_t));
            std::memcpy(result.data(), str.data(), str.size());
            return result;
        }
    };

    template<> struct param_traits<cl_device_info, CL_DEVICE_MAX_WORK_GROUP_SIZE> : public device_type {
        enum { value = CL_DEVICE_MAX_WORK_GROUP_SIZE };
        using type = size_t;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
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

    template<> struct param_traits<cl_command_queue_info, CL_QUEUE_CONTEXT> : public command_queue_type {
        enum { value = CL_QUEUE_CONTEXT };
        using type = cl_context;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };

    template<> struct param_traits<cl_command_queue_info, CL_QUEUE_DEVICE> : public command_queue_type {
        enum { value = CL_QUEUE_DEVICE };
        using type = cl_device_id;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };

    template<> struct param_traits<cl_command_queue_info, CL_QUEUE_REFERENCE_COUNT> : public command_queue_type {
        enum { value = CL_QUEUE_REFERENCE_COUNT };
        using type = cl_uint;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };

    template<> struct param_traits<cl_command_queue_info, CL_QUEUE_PROPERTIES> : public command_queue_type {
        enum { value = CL_QUEUE_PROPERTIES };
        using type = cl_command_queue_properties;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };

    /*------------------------------------------------------------------------------------------*/
    
      template<> struct param_traits<cl_program_info, CL_PROGRAM_REFERENCE_COUNT> : public program_type {
        enum { value = CL_PROGRAM_REFERENCE_COUNT };
        using type = cl_uint;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };
    
    template<> struct param_traits<cl_program_info, CL_PROGRAM_CONTEXT> : public program_type {
        enum { value = CL_PROGRAM_CONTEXT };
        using type = cl_context;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };
    
    template<> struct param_traits<cl_program_info, CL_PROGRAM_NUM_DEVICES> : public program_type {
        enum { value = CL_PROGRAM_NUM_DEVICES };
        using type = cl_uint;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };
    
    template<> struct param_traits<cl_program_info, CL_PROGRAM_DEVICES> : public program_type {
        enum { value = CL_PROGRAM_DEVICES };
        using type = std::vector<cl_device_id>;
        static type to_type(std::string &str) {
            type devices(str.size() / sizeof(cl_device_id));
            std::memcpy(devices.data(), str.data(), str.size());
            return devices;
        }
    };
    
    template<> struct param_traits<cl_program_info, CL_PROGRAM_SOURCE> : public program_type {
        enum { value = CL_PROGRAM_SOURCE };
        using type = std::string;
        static type to_type(std::string &str) { return str; }
    };
    
    template<> struct param_traits<cl_program_info, CL_PROGRAM_BINARY_SIZES> : public program_type {
        enum { value = CL_PROGRAM_BINARY_SIZES };
        using type = std::vector<size_t>;
        static type to_type(std::string &str) {
            type sizes(str.size() / sizeof(size_t));
            std::memcpy(sizes.data(), str.data(), str.size());
            return sizes;
        }
    };
    
    template<> struct param_traits<cl_program_info, CL_PROGRAM_BINARIES> : public program_type {
        enum { value = CL_PROGRAM_BINARIES };
        using type = std::vector<std::vector<unsigned char>>;
        static type to_type(std::string &str) {
            return {std::vector<unsigned char>(str.begin(), str.end())};
        }
    };
    
    template<> struct param_traits<cl_program_info, CL_PROGRAM_NUM_KERNELS> : public program_type {
        enum { value = CL_PROGRAM_NUM_KERNELS };
        using type = cl_uint;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };
    
    template<> struct param_traits<cl_program_info, CL_PROGRAM_KERNEL_NAMES> : public program_type {
        enum { value = CL_PROGRAM_KERNEL_NAMES };
        using type = std::string;
        static type to_type(std::string &str) { return str; }
    };

    /*------------------------------------------------------------------------------------------*/
    
    template<> struct param_traits<cl_kernel_info, CL_KERNEL_FUNCTION_NAME> : public kernel_type {
        enum { value = CL_KERNEL_FUNCTION_NAME };
        using type = std::string;
        static type to_type(std::string &str) { return str; }
    };
    
    template<> struct param_traits<cl_kernel_info, CL_KERNEL_NUM_ARGS> : public kernel_type {
        enum { value = CL_KERNEL_NUM_ARGS };
        using type = cl_uint;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };
    
    template<> struct param_traits<cl_kernel_info, CL_KERNEL_REFERENCE_COUNT> : public kernel_type {
        enum { value = CL_KERNEL_REFERENCE_COUNT };
        using type = cl_uint;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };
    
    template<> struct param_traits<cl_kernel_info, CL_KERNEL_CONTEXT> : public kernel_type {
        enum { value = CL_KERNEL_CONTEXT };
        using type = cl_context;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };
    
    template<> struct param_traits<cl_kernel_info, CL_KERNEL_PROGRAM> : public kernel_type {
        enum { value = CL_KERNEL_PROGRAM };
        using type = cl_program;
        static type to_type(std::string &str) {
            return *reinterpret_cast<const type*>(str.data());
        }
    };
    
    template<> struct param_traits<cl_kernel_info, CL_KERNEL_ATTRIBUTES> : public kernel_type {
        enum { value = CL_KERNEL_ATTRIBUTES };
        using type = std::string;
        static type to_type(std::string &str) { return str; }
    };

    /*------------------------------------------------------------------------------------------*/

    template<> struct param_traits<cl_mem_info, CL_MEM_TYPE> : public memory_type {
        enum { value = CL_MEM_TYPE };
        using type = cl_mem_object_type;
        static type to_type(std::string &str) {
            type result;
            std::copy_n(str.data(), sizeof(type), reinterpret_cast<char*>(&result));
            return result;
        }
    };

    template<> struct param_traits<cl_mem_info, CL_MEM_FLAGS> : public memory_type {
        enum { value = CL_MEM_FLAGS };
        using type = cl_mem_flags;
        static type to_type(std::string &str) {
            type result;
            std::copy_n(str.data(), sizeof(type), reinterpret_cast<char*>(&result));
            return result;
        }
    };

    template<> struct param_traits<cl_mem_info, CL_MEM_SIZE> : public memory_type {
        enum { value = CL_MEM_SIZE };
        using type = size_t;
        static type to_type(std::string &str) {
            type result;
            std::copy_n(str.data(), sizeof(type), reinterpret_cast<char*>(&result));
            return result;
        }
    };

    template<> struct param_traits<cl_mem_info, CL_MEM_HOST_PTR> : public memory_type {
        enum { value = CL_MEM_HOST_PTR };
        using type = void*;
        static type to_type(std::string &str) {
            type result;
            std::copy_n(str.data(), sizeof(type), reinterpret_cast<char*>(&result));
            return result;
        }
    };

    template<> struct param_traits<cl_mem_info, CL_MEM_MAP_COUNT> : public memory_type {
        enum { value = CL_MEM_MAP_COUNT };
        using type = cl_uint;
        static type to_type(std::string &str) {
            type result;
            std::copy_n(str.data(), sizeof(type), reinterpret_cast<char*>(&result));
            return result;
        }
    };

    template<> struct param_traits<cl_mem_info, CL_MEM_REFERENCE_COUNT> : public memory_type {
        enum { value = CL_MEM_REFERENCE_COUNT };
        using type = cl_uint;
        static type to_type(std::string &str) {
            type result;
            std::copy_n(str.data(), sizeof(type), reinterpret_cast<char*>(&result));
            return result;
        }
    };

    template<> struct param_traits<cl_mem_info, CL_MEM_CONTEXT> : public memory_type {
        enum { value = CL_MEM_CONTEXT };
        using type = cl_context;
        static type to_type(std::string &str) {
            type result;
            std::copy_n(str.data(), sizeof(type), reinterpret_cast<char*>(&result));
            return result;
        }
    };

    /*------------------------------------------------------------------------------------------*/

    template<typename ObjT> struct obj_info_type;
    
    template<> struct obj_info_type<cl_platform_id>   { using type = cl_platform_info;      };
    template<> struct obj_info_type<cl_device_id>     { using type = cl_device_info;        };
    template<> struct obj_info_type<cl_context>       { using type = cl_context_info;       };
    template<> struct obj_info_type<cl_command_queue> { using type = cl_command_queue_info; };
    template<> struct obj_info_type<cl_program>       { using type = cl_program_info;       };
    template<> struct obj_info_type<cl_kernel>        { using type = cl_kernel_info;        };
    template<> struct obj_info_type<cl_mem>           { using type = cl_mem_info;           };

    /*------------------------------------------------------------------------------------------*/

    template <typename ObjT>
    class wrapper_t {
    protected:
        ObjT obj_;

        using InfoT = typename obj_info_type<ObjT>::type;

    public:
        wrapper_t(ObjT obj = NULL) : obj_(obj) {
            retain();
        }

        wrapper_t(const wrapper_t &rhs) {
            if (this == &rhs)
                return;

            release();
            obj_ = rhs.obj_;
            retain();
        }

        wrapper_t &operator=(const wrapper_t &rhs) {
            if (this == &rhs)
                return *this;

            wrapper_t new_obj{rhs};
            std::swap(*this, new_obj);
            return *this;
        }

        wrapper_t(wrapper_t &&rhs) noexcept : obj_(std::exchange(rhs.obj_, nullptr)) {}

        wrapper_t &operator=(wrapper_t &&rhs) noexcept {
            obj_ = std::exchange(rhs.obj_, nullptr);
            return *this;
        }

              ObjT& operator()()       { return obj_; }
        const ObjT& operator()() const { return obj_; }

        ObjT obj       () const { return obj_; }
        ObjT get_retain() const { retain(); return obj_; }

        cl_int retain () const { return ReferenceHandler<ObjT>::retain (obj_); }
        cl_int release() const { return ReferenceHandler<ObjT>::release(obj_); }

        template <InfoT NameT>
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