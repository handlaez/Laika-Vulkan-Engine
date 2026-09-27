#include "se_hierarchy_panel.hpp"
#include "imgui.h"
#include "src/scene/le_scene.hpp"
#include "src/editor/se_editor_selection.hpp"
#include "src/editor/se_mode_controller.hpp"

namespace se {
    HierarchyPanel::HierarchyPanel(EditorContext& context)
        : context_(context)
    {
    }

    void HierarchyPanel::onImGuiRender()
    {
        ImGui::Begin("Hierarchy");

        if (!context_.scene || !context_.modeController)
        {
            ImGui::TextUnformatted("No project loaded.");
            ImGui::End();
            return;
        }

        auto& scene = *context_.scene;
        auto& selection = context_.selection;

        if (!canEdit_)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Add Cube"))
        {
            const auto id = scene.addActor(0, 0);
            selection.select(id);
        }

        ImGui::SameLine();

        const bool hasSelection = selection.hasSelection();

        if (!hasSelection)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Delete"))
        {
            const auto id = selection.getSelectedActor();

            if (id && scene.removeActorById(*id))
            {
                selection.clear();
            }
        }

        if (!hasSelection)
        {
            ImGui::EndDisabled();
        }

        ImGui::Separator();

        const auto rootActors = scene.getRootActors();

        for (const auto id : rootActors)
        {
            drawActorTree(id);
        }

        if (!canEdit_)
        {
            ImGui::EndDisabled();
        }

        ImGui::End();
    }

    void HierarchyPanel::onUpdate()
    {
        if (!context_.modeController)
        {
            canEdit_ = false;
            return;
        }

        canEdit_ = context_.modeController->getState() == PlayState::Edit;
    }

    void HierarchyPanel::drawActorTree(le::LeActor::id_t id)
    {
        auto& scene = *context_.scene;
        auto& selection = context_.selection;

        const auto* actor = scene.getActorById(id);

        if (!actor)
        {
            return;
        }

        const auto children = scene.getChildren(id);
        const bool hasChildren = !children.empty();
        const bool selected = selection.getSelectedActor() == id;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

        if (!hasChildren)
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        if (selected)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        const std::string label = "Actor " + std::to_string(actor->getId()) + "##actor_" + std::to_string(actor->getId());

        const bool open = ImGui::TreeNodeEx(label.c_str(), flags);

        if (ImGui::IsItemClicked())
        {
            selection.select(id);
        }

        // drag actor
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("ACTOR_ID", &id, sizeof(id));
            ImGui::TextUnformatted(("Actor " + std::to_string(id)).c_str());

            ImGui::EndDragDropSource();
        }

        // drop actor onto this actor to make it a child.
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ACTOR_ID"))
            {
                const auto draggedId = *static_cast<const le::LeActor::id_t*>(payload->Data);
                scene.setParent(draggedId, id);
            }

            ImGui::EndDragDropTarget();
        }

        // right-click context menu.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Select"))
            {
                selection.select(id);
            }

            if (actor->getParentId().has_value())
            {
                if (ImGui::MenuItem("Unparent"))
                {
                    scene.clearParent(id);
                }
            }

            ImGui::EndPopup();
        }

        if (open)
        {
            for (const auto childId : children)
            {
                drawActorTree(childId);
            }

            ImGui::TreePop();
        }
    }
}