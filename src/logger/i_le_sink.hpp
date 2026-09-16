#ifndef I_LE_SINK_HPP
#define I_LE_SINK_HPP

#include "le_log_record.hpp"

namespace le::log
{
    class Sink
    {
    public:
        virtual ~Sink() = default;

        virtual void write(const Record& record) = 0;
    };
}

#endif 