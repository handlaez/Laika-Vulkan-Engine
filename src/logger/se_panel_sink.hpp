#ifndef SE_PANEL_SINK_HPP
#define SE_PANEL_SINK_HPP

#include "i_le_sink.hpp"
#include "le_log_record.hpp"

#include <memory>
#include <vector>

namespace se
{
    class PanelSink final : public le::log::Sink
    {
    public:
        using Records = std::shared_ptr<std::vector<le::log::Record>>;

        explicit PanelSink(Records records);

        void write(const le::log::Record& record) override;

    private:
        Records records_;
    };
}

#endif