#include "se_panel_sink.hpp"

#include <utility>

namespace se
{
    PanelSink::PanelSink(Records records) : records_(std::move(records))
    {
    }

    void PanelSink::write(const le::log::Record& record)
    {
        records_->push_back(record);
    }
}