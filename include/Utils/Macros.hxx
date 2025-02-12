#ifndef LIT_UTILS_MACROS_HXX
#define LIT_UTILS_MACROS_HXX

#define SMALLIF(expr, r1, r2) ((expr ? r1 : r2 ))

#define UNUSED(var) ((void)var)

#define ASSERT(expr, msg) \
    do { \
        if (!(expr)) { \
            spdlog::log(spdlog::level::critical, msg); \
            std::abort(); \
        } \
    } while (false)

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#define LIT_SPDLOG_MAKE_FILE_SINK(file) std::make_shared<spdlog::sinks::basic_file_sink_mt>(file, true)
#define LIT_SPDLOG_MAKE_STDOUT_SINK() std::make_shared<spdlog::sinks::stdout_color_sink_mt>()

#define ENV(varname) std::getenv(varname)

#if defined(WIN32)
#define LIT_EXPORT __declspec(dllexport)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#else
#define LIT_EXPORT __attribute__((visibility("default")))
#endif 
#endif // LIT_UTILS_MACROS_HXX
