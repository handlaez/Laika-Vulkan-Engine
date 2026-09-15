#include "le_console_sink.hpp"

#include <iostream>

namespace le::log
{
    void ConsoleSink::write(const Record& record)
    {
        std::cout << record.message << '\n';
    }
}