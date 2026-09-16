#ifndef I_LE_CONSOLE_SINK_HPP
#define I_LE_CONSOLE_SINK_HPP

#include "i_le_sink.hpp"

namespace le::log
{
    class ConsoleSink final : public Sink
    {
    public:
        void write(const Record& record) override;
    };
}

#endif