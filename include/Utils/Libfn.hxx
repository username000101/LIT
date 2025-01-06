#ifndef LIT_UTILS_LIBFN_HXX
#define LIT_UTILS_LIBFN_HXX

#include <dlfcn.h>

#include <string>

#include <spdlog/spdlog.h>

namespace utils {

    template <typename FunctionSignature, typename... FunctionArguments> FunctionSignature get_function
    (void* handle, const std::string& function_name, FunctionArguments&&... function_args) {
        if (!handle) {
            spdlog::log(spdlog::level::err,
                "{}: Failed to get function: cannot open lib: {}", __FUNCTION__, dlerror());
            return nullptr;
        }

        FunctionSignature function = (FunctionSignature)dlsym(handle, function_name.c_str());
        if (!function) {
            spdlog::log(spdlog::level::err,
                "{}: Failed to get function: cannot sym function: {}", __FUNCTION__, dlerror());
            return nullptr;
        }

        if (!std::is_invocable_v<FunctionSignature, FunctionArguments...>) {
            spdlog::log(spdlog::level::err,
                "{}: Failed to get function: function signature is invalid", __FUNCTION__);
            return nullptr;
        }
        return function;
    }

    template <typename FunctionSignature> FunctionSignature get_function
    (void* handle, const std::string& function_name) {
        if (!handle) {
            spdlog::log(spdlog::level::err,
                "{}: Failed to get function: cannot open lib: {}", __FUNCTION__,dlerror());
            return nullptr;
        }

        FunctionSignature function = (FunctionSignature)dlsym
        (handle, function_name.c_str());
        if (!function) {
            spdlog::log(spdlog::level::err,
                "{}: Failed to get function: cannot sym function: {}", __FUNCTION__, dlerror());
            return nullptr;
        }

        return function;
    }
}

#endif // LIT_UTILS_LIBFN_HXX
