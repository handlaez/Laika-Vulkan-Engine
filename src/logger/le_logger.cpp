#include "le_logger.hpp"

#include <utility>

namespace le::log
{
    void Logger::addSink(std::unique_ptr<Sink> sink)
    {
        sinks_.push_back(std::move(sink));
    }

    void Logger::write(
        Level level,
        Category category,
        std::string_view message,
        std::source_location location)
    {
        Record entry{
            .level = level,
            .category = category,
            .message = std::string{message},
            .location = location
        };

        for (auto& sink : sinks_)
        {
            sink->write(entry);
        }
    }
}