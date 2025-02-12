#ifndef LIT_UTILS_LIBFN_HXX
#define LIT_UTILS_LIBFN_HXX

#include <filesystem>
#include <string>

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

namespace utils {
#ifdef unix
#include <dlfcn.h>
    inline void* open_library(const std::string& path) {
        if (!std::filesystem::exists(path)) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to open lib: file not found",
                        __PRETTY_FUNCTION__);
            return nullptr;
        }

        void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (!handle) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to open lib: {}",
                        __PRETTY_FUNCTION__,
                        dlerror());
            return nullptr;
        }

        return handle;
    }

    inline void close_library(void* handle) { if (handle) dlclose(handle); }

    template <typename FunctionSignature, typename... FunctionArguments> FunctionSignature get_symbol
    (void* handle, const std::string& function_name, FunctionArguments&&... function_args) {
        if (!handle) {
            spdlog::log(spdlog::level::err,
                "{}: Failed to get function: cannot open lib: {}",
                __PRETTY_FUNCTION__,
                dlerror());
            return nullptr;
        }

        if (!std::is_invocable_v<FunctionSignature, FunctionArguments...>) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to get function: function signature is invalid",
                        __PRETTY_FUNCTION__);
            return nullptr;
        }

        FunctionSignature function = (FunctionSignature)dlsym(handle, function_name.c_str());
        if (!function) {
            spdlog::log(spdlog::level::err,
                "{}: Failed to get function: cannot sym function: {}",
                __PRETTY_FUNCTION__,
                dlerror());
            return nullptr;
        }
        return function;
    }

    template <typename FunctionSignature> FunctionSignature get_symbol
    (void* handle, const std::string& function_name) {
        [[unlikely]] if (!handle) {
            spdlog::log(spdlog::level::err,
                "{}: Failed to get function: cannot open lib: {}",
                __PRETTY_FUNCTION__,
                dlerror());
            return nullptr;
        }

        FunctionSignature function = (FunctionSignature)dlsym(handle, function_name.c_str());
        if (!function) {
            spdlog::log(spdlog::level::err,
                "{}: Failed to get function: cannot sym function: {}",
                __PRETTY_FUNCTION__,
                dlerror());
            return nullptr;
        }

        return function;
    }
#else
#include <windows.h>

    inline HMODULE open_library(const std::string& path) {
        if (!std::filesystem::exists(path)) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to open lib: file not found",
                        __PRETTY_FUNCTION__);
            return nullptr;
        }

        HMODULE handle = LoadLibrary(path.c_str());
        if (!handle) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to open lib: {}",
                        __PRETTY_FUNCTION__,
                        GetLastError());
            return nullptr;
        }

        return handle;
    }

    inline void close_library(HMODULE handle) { if (handle) FreeLibrary(handle); }

    template <typename FunctionSignature, typename... FunctionArguments> FunctionSignature get_symbol
    (HMODULE handle, const std::string& function_name, FunctionArguments&&... function_args) {
        if (!handle) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to get function: cannot open lib: {}",
                        __PRETTY_FUNCTION__,
                        GetLastError());
            return nullptr;
        }

        if (!std::is_invocable_v<FunctionSignature, FunctionArguments...>) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to get function: function signature is invalid",
                        __PRETTY_FUNCTION__);
            return nullptr;
        }

        FunctionSignature function = (FunctionSignature)GetProcAddress(handle, function_name.c_str());
        if (!function) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to get function: cannot sym function: {}",
                        __PRETTY_FUNCTION__,
                        GetLastError());
            return nullptr;
        }

        return function;
    }

    template <typename FunctionSignature> FunctionSignature get_symbol
        (HMODULE handle, const std::string& function_name) {
        if (!handle) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to get function: cannot open lib: {}",
                        __PRETTY_FUNCTION__,
                        GetLastError());
            return nullptr;
        }

        FunctionSignature function = (FunctionSignature)GetProcAddress(handle, function_name.c_str());
        if (!function) {
            spdlog::log(spdlog::level::err,
                        "{}: Failed to get function: cannot sym function: {}",
                        __PRETTY_FUNCTION__,
                        GetLastError());
            return nullptr;
        }

        return function;
    }
#endif
}

#endif // LIT_UTILS_LIBFN_HXX
