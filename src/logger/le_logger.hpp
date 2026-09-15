#ifndef LE_LOGGER_HPP
#define LE_LOGGER_HPP

#include "i_le_sink.hpp"
#include "le_log_record.hpp"

#include <memory>
#include <source_location>
#include <string_view>
#include <vector>
#include <string>

namespace le::log
{
    class logger
    {
    public:
        void addSink(std::unique_ptr<Sink> sink);
        void write(Level level, Category category, std::string_view message, std::source_location location = std::source_location::current());

    private:
        std::vector<std::unique_ptr<Sink>> sinks_;
    };
}

#endif