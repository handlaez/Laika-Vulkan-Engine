#ifndef CONSOLE_PANEL_HPP
#define CONSOLE_PANEL_HPP

#include "i_editor_panel.hpp"
#include "src/logger/le_log_record.hpp"

#include <memory>
#include <vector>

namespace se
{
    class ConsolePanel : public IEditorPanel
    {
    public:
        using Records = std::shared_ptr<std::vector<le::log::Record>>;

        explicit ConsolePanel(Records records);

        void onImGuiRender() override;
        void onUpdate() override;

    private:
        Records records_;
    };
}

#endif