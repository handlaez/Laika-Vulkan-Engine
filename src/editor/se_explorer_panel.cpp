#include "se_explorer_panel.hpp"

#include <imgui.h>

#include <algorithm>

namespace se {

    ExplorerPanel::ExplorerPanel() : m_rootDirectory("assets")
    {
        if (!std::filesystem::exists(m_rootDirectory))
        {
            m_rootDirectory = std::filesystem::current_path();
        }
    }

    void ExplorerPanel::onUpdate()
    {
        if (m_shouldRefresh)
        {
            refresh();
            m_shouldRefresh = false;
        }
    }

    void ExplorerPanel::onImGuiRender()
    {
        ImGui::Begin("Explorer");

        // toolbar
        if (ImGui::Button("Refresh"))
        {
            m_shouldRefresh = true;
        }

        ImGui::SameLine();

        ImGui::TextUnformatted(m_rootDirectory.string().c_str());

        ImGui::Separator();

        // root dir
        if (std::filesystem::exists(m_rootDirectory))
        {
            drawDirectory(m_rootDirectory);
        }
        else
        {
            ImGui::TextUnformatted("Root directory does not exist.");
        }

        ImGui::End();
    }

    void ExplorerPanel::drawDirectory(const std::filesystem::path& path)
    {
        try
        {
            std::vector<std::filesystem::directory_entry> entries;

            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                entries.push_back(entry);
            }

            // directories first, then files.
            std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b)
                {
                    const bool aDirectory = a.is_directory();
                    const bool bDirectory = b.is_directory();

                    if (aDirectory != bDirectory)
                        return aDirectory > bDirectory;

                    return a.path().filename().string() < b.path().filename().string();
                }
            );

            for (const auto& entry : entries)
            {
                const auto& entryPath = entry.path();
                const std::string name = entryPath.filename().string();

                if (entry.is_directory())
                {
                    ImGuiTreeNodeFlags flags =
                        ImGuiTreeNodeFlags_OpenOnArrow |
                        ImGuiTreeNodeFlags_SpanAvailWidth;

                    const bool isOpen = ImGui::TreeNodeEx(name.c_str(), flags);

                    if (ImGui::IsItemClicked())
                    {
                        m_selectedPath = entryPath;
                    }

                    if (isOpen)
                    {
                        drawDirectory(entryPath);
                        ImGui::TreePop();
                    }
                }
                else
                {
                    const bool selected = m_selectedPath == entryPath;

                    if (ImGui::Selectable(name.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        m_selectedPath = entryPath;
                    }

                    // fouble-click hook.
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        // AssetManager::open(entryPath);
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error&)
        {
            ImGui::TextUnformatted("<Unable to read directory>");
        }
    }

    void ExplorerPanel::refresh()
    {
        // this implementation scans the filesystem directly during rendering, so there is no cache to rebuild yet.
        //
        // Keeping this function here to later add a cached asset tree without changing the panel interface.
    }
}