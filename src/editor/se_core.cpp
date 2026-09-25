#include "src/editor/se_core.hpp"
#include "src/laika_app.hpp"
#include "src/logger/le_logger.hpp"
#include "src/logger/le_console_sink.hpp"
#include "src/logger/se_panel_sink.hpp"

#include "src/editor/panel/se_scene_panel.hpp"
#include "src/editor/panel/se_hierarchy_panel.hpp"
#include "src/editor/panel/se_console_panel.hpp"
#include "src/editor/panel/se_inspector_panel.hpp"
#include "src/editor/panel/se_explorer_panel.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <array>
#include <stdexcept>
#include <imgui_internal.h>

namespace se {

    SeCore::SeCore()
        : leCore_{},
        editorScene_{ std::make_unique<le::LeScene>(leCore_.getDevice(), leCore_.getResources()) },
        laikaApp_{},
        modeController_{ std::make_unique<ModeController>(*editorScene_, laikaApp_) },
        consoleLogRecords_{ std::make_shared<std::vector<le::log::Record>>() }
    {
        initImGui();

        editorPanels_.push_back(std::make_unique<ScenePanel>(leCore_, &sceneTextureDescriptorSet_, &sceneViewportHovered_));
        editorPanels_.push_back(std::make_unique<ConsolePanel>(consoleLogRecords_));
        editorPanels_.push_back(std::make_unique<ExplorerPanel>("."));
        editorPanels_.push_back(std::make_unique<InspectorPanel>(*editorScene_, editorSelection_, *modeController_));
        editorPanels_.push_back(std::make_unique<HierarchyPanel>(*editorScene_, editorSelection_, *modeController_));

        leCore_.getRenderer().setSceneRenderTargetRecreatedCallback(
            [this](le::LeSceneRenderTarget& sceneTarget)
            {
                recreateSceneTexture(sceneTarget);
            });

        leCore_.setRenderOverlay(
            [this](VkCommandBuffer commandBuffer)
            {
                renderImGui(commandBuffer);
            }
        );
    }

    se::SeCore::~SeCore()
    {
        vkDeviceWaitIdle(leCore_.getDevice().device());
        shutdownImGui();
    }

    void se::SeCore::run()
    {
        laikaApp_.onLoad(*editorScene_);

        ::le::log::Logger logger;
        logger.addSink(std::make_unique<::le::log::ConsoleSink>());
        logger.addSink(std::make_unique<::se::PanelSink>(consoleLogRecords_));

        logger.write(::le::log::Level::info, ::le::log::Category::editor, "Poyekhali!");

        while (!leCore_.getWindow().shouldClose())
        {
            leCore_.beginFrame();

            Utils::checkKeys(leCore_.getWindow().getGLFWwindow());

            modeController_->update(leCore_.getFrameInfo(), sceneViewportHovered_);

            updateEditorCamera();
            updateEditor();

            leCore_.render(modeController_->getActiveScene());

            renderEditor();

            leCore_.endFrame();
        }

        modeController_->stop();
    }

    void SeCore::createImGuiDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 1> poolSizes{};

        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = 1000;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        if (vkCreateDescriptorPool(
            leCore_.getDevice().device(),
            &poolInfo,
            nullptr,
            &imguiDescriptorPool_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create ImGui descriptor pool");
        }
    }

    void SeCore::initImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = "sputnik_imgui.ini";
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NoMouseCursorChange;

        ImGui::StyleColorsDark();

        GLFWwindow* window = leCore_.getWindow().getGLFWwindow();

        ImGui_ImplGlfw_InitForVulkan(window, true);

        const auto queueFamilies = leCore_.getDevice().findPhysicalQueueFamilies();

        ImGui_ImplVulkan_InitInfo initInfo{};

        initInfo.ApiVersion = VK_API_VERSION_1_4;
        initInfo.Instance = leCore_.getDevice().getVkInstance();
        initInfo.PhysicalDevice = leCore_.getDevice().getPhysicalDevice();
        initInfo.Device = leCore_.getDevice().device();
        initInfo.QueueFamily = queueFamilies.graphicsFamily;
        initInfo.Queue = leCore_.getDevice().graphicsQueue();

        // Let ImGui create its own descriptor pool.
        initInfo.DescriptorPool = VK_NULL_HANDLE;
        initInfo.DescriptorPoolSize = 512;
        initInfo.MinImageCount = 2;
        initInfo.ImageCount = le::LeSwapchain::MAX_FRAMES_IN_FLIGHT;
        initInfo.PipelineCache = VK_NULL_HANDLE;
        initInfo.PipelineInfoMain.RenderPass = leCore_.getRenderer().getSwapchainRenderPass();
        initInfo.PipelineInfoMain.Subpass = 0;
        initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.UseDynamicRendering = false;
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = nullptr;
        initInfo.MinAllocationSize = 1024 * 1024;

        if (!ImGui_ImplVulkan_Init(&initInfo))
        {
            throw std::runtime_error("Failed to initialize ImGui Vulkan backend!");
        }

        recreateSceneTexture(leCore_.getRenderer().getSceneRenderTarget());
    }

    void SeCore::shutdownImGui()
    {
        if (sceneTextureDescriptorSet_ != VK_NULL_HANDLE)
        {
            ImGui_ImplVulkan_RemoveTexture(sceneTextureDescriptorSet_);
            sceneTextureDescriptorSet_ = VK_NULL_HANDLE;
        }

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void SeCore::renderImGui(VkCommandBuffer commandBuffer)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiIO& io = ImGui::GetIO();

        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("Sputnik Editor", nullptr, windowFlags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspaceId = ImGui::GetID("SputnikDockspace");

        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        if (!dockspaceInitialized_) {
            initializeDockspace();
            dockspaceInitialized_ = true;
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    glfwSetWindowShouldClose(
                        leCore_.getWindow().getGLFWwindow(),
                        GLFW_TRUE
                    );
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Lighting", "3", &Utils::lightingEnabled);
                ImGui::MenuItem("Textures", "4", &Utils::texturesEnabled);
                ImGui::MenuItem("Wireframe", "5", &Utils::wireframeEnabled);
                ImGui::MenuItem("Skybox", "6", &Utils::skyboxEnabled);
                ImGui::MenuItem("Hitboxes", "7", &Utils::hitboxesEnabled);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window")) {
                ImGui::MenuItem("Scene");
                ImGui::MenuItem("Inspector");
                ImGui::MenuItem("Hierarchy");
                ImGui::MenuItem("Console");

                ImGui::EndMenu();
            }

            renderPlayToolbar();
            ImGui::EndMenuBar();
        }

        ImGui::End();

        renderEditorWindows();

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }

    void SeCore::renderEditorWindows()
    {
        for (auto& panel : editorPanels_) {
            panel->onImGuiRender();
        }
    }

    void SeCore::renderPlayToolbar()
    {
        const auto state = modeController_->getState();

        const char* primaryLabel =
            state == PlayState::Edit ? "Run" :
            state == PlayState::Run ? "Pause" :
            "Resume";

        const char* statusLabel =
            state == PlayState::Edit ? "[Editing]" :
            state == PlayState::Run ? "[Running]" :
            "[Paused]";

        const bool showRestartStop = state != PlayState::Edit;

        const ImGuiStyle& style = ImGui::GetStyle();

        auto buttonWidth = [&](const char* label)
            {
                return ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
            };

        float totalWidth = buttonWidth(primaryLabel);

        if (showRestartStop)
        {
            totalWidth += style.ItemSpacing.x;
            totalWidth += buttonWidth("Restart");

            totalWidth += style.ItemSpacing.x;
            totalWidth += buttonWidth("Stop");
        }

        totalWidth += style.ItemSpacing.x;
        totalWidth += ImGui::CalcTextSize(statusLabel).x;

        const float availableWidth = ImGui::GetWindowWidth();
        const float centeredX = (availableWidth - totalWidth) * 0.5f;

        ImGui::SetCursorPosX(centeredX);

        // Primary button
        if (ImGui::Button(primaryLabel))
        {
            switch (state)
            {
            case PlayState::Edit:
                modeController_->run();
                break;

            case PlayState::Run:
                modeController_->pause();
                break;

            case PlayState::Pause:
                modeController_->resume();
                break;
            }
        }

        if (showRestartStop)
        {
            ImGui::SameLine();

            if (ImGui::Button("Restart"))
            {
                modeController_->restart();
            }

            ImGui::SameLine();

            if (ImGui::Button("Stop"))
            {
                modeController_->stop();
            }
        }

        ImGui::SameLine();
        ImGui::TextUnformatted(statusLabel);
    }

    void SeCore::initializeDockspace()
    {
        ImGuiID dockspaceId = ImGui::GetID("SputnikDockspace");

        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);

        ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

        ImGuiID mainDock = dockspaceId;
        ImGuiID leftDock;
        ImGuiID rightDock;
        ImGuiID bottomDock;

        leftDock = ImGui::DockBuilderSplitNode(
            mainDock,
            ImGuiDir_Left,
            0.20f,
            nullptr,
            &mainDock
        );

        rightDock = ImGui::DockBuilderSplitNode(
            mainDock,
            ImGuiDir_Right,
            0.25f,
            nullptr,
            &mainDock
        );

        bottomDock = ImGui::DockBuilderSplitNode(
            mainDock,
            ImGuiDir_Down,
            0.25f,
            nullptr,
            &mainDock
        );

        ImGui::DockBuilderDockWindow("Hierarchy", leftDock);
        ImGui::DockBuilderDockWindow("Inspector", rightDock);
        ImGui::DockBuilderDockWindow("Console", bottomDock);
        ImGui::DockBuilderDockWindow("Scene", mainDock);

        ImGui::DockBuilderFinish(dockspaceId);
    }

    void SeCore::recreateSceneTexture(le::LeSceneRenderTarget& sceneTarget)
    {
        if (sceneTextureDescriptorSet_ != VK_NULL_HANDLE)
        {
            ImGui_ImplVulkan_RemoveTexture(sceneTextureDescriptorSet_);
            sceneTextureDescriptorSet_ = VK_NULL_HANDLE;
        }

        sceneTextureDescriptorSet_ = ImGui_ImplVulkan_AddTexture(
            sceneTarget.getColorSampler(),
            sceneTarget.getColorImageView(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
    }

    void SeCore::updateEditor()
    {
        for (auto& panel : editorPanels_) {
            panel->onUpdate();
        }
    }

    void SeCore::renderEditor()
    {
    }

    void SeCore::updateEditorCamera()
    {
        if (modeController_->getState() != PlayState::Edit)
        {
            return;
        }

        auto& frameInfo = leCore_.getFrameInfo();
        auto& cameraObject = editorScene_->getCameraObject();
        auto& camera = editorScene_->getCamera();

        editorCameraController_.moveInPlaneXZ(
            frameInfo.window,
            frameInfo.deltaTime,
            cameraObject,
            sceneViewportHovered_
        );

        camera.setPerspectiveProjection(glm::radians(50.f), frameInfo.aspect, 0.1f, 100.f);
        camera.setView(cameraObject.transform.translation, cameraObject.transform.rotation);
    }

    bool SeCore::openProject(const std::filesystem::path& projectFile)
    {
        closeProject();

        if (!projectManager_.loadProject(projectFile)) {
            return false;
        }

        leCore_.getResources().reset();

        editorScene_ = std::make_unique<le::LeScene>(leCore_.getDevice(), leCore_.getResources());
        modeController_ = std::make_unique<ModeController>(*editorScene_, laikaApp_);

        editorSelection_.clear();

        laikaApp_.onLoad(*editorScene_);

        return true;
    }

    void SeCore::closeProject()
    {
        if (modeController_) {
            modeController_->stop();
        }

        editorSelection_.clear();

        modeController_.reset();
        editorScene_.reset();

        leCore_.getResources().reset();

        projectManager_.unloadProject();
    }
} // se