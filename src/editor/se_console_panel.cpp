#include "se_console_panel.hpp"

#include "imgui.h"

#include <utility>

se::ConsolePanel::ConsolePanel(Records records)
    : records_(std::move(records))
{
}

void se::ConsolePanel::onImGuiRender()
{
    ImGui::Begin("Console");

    if (ImGui::Button("Clear"))
    {
        records_->clear();
    }

    ImGui::Separator();

    ImGui::BeginChild("LogScroll");

    const bool atBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f;

    for (const auto& record : *records_)
    {
        ImGui::TextUnformatted(record.message.c_str());
    }

    if (atBottom && !records_->empty())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}

void se::ConsolePanel::onUpdate()
{
}