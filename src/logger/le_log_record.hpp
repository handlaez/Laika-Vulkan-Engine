#ifndef LE_LOG_RECORD_HPP
#define LE_LOG_RECORD_HPP

#include <source_location>
#include <string>

namespace le::log
{
    enum class Level
    {
        trace,
        debug,
        info,
        warning,
        error,
        fatal
    };

    enum class Category
    {
        core,
        render,
        asset,
        physics,
        audio,
        editor
    };

    struct Record
    {
        Level level;
        Category category;

        std::string message;
        std::source_location location;
    };
}

#endif