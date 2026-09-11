#include "src/editor/se_core.hpp"
#include "src/demo_app.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <array>
#include <stdexcept>
#include <imgui_internal.h>

namespace se {

    se::SeCore::SeCore() 
        : leCore_{}, currentScene_ { leCore_.getDevice(), leCore_.getResources() }
    {
        initImGui();

        leCore_.getRenderer().setSceneRenderTargetRecreatedCallback(
            [this](LeSceneRenderTarget& sceneTarget)
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
        DemoApp demoApp;
        demoApp.onStart(currentScene_);

        while (!leCore_.getWindow().shouldClose())
        {
            leCore_.beginFrame();

            Utils::checkKeys(leCore_.getWindow().getGLFWwindow());

            demoApp.onUpdate(currentScene_, leCore_.getFrameInfo(), sceneViewportActive_);
            updateEditor();

            leCore_.render(currentScene_);
            renderEditor();

            leCore_.endFrame();
        }

        demoApp.onShutdown();
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
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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
        initInfo.ImageCount = LeSwapchain::MAX_FRAMES_IN_FLIGHT;

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

            ImGui::EndMenuBar();
        }

        ImGui::End();

        renderEditorWindows();

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }

    void SeCore::renderEditorWindows()
    {
        ImGui::Begin("Scene");

        // checking if scene viewport should capture the mouse
        sceneViewportHovered_ = ImGui::IsWindowHovered();
        const bool viewportClicked = sceneViewportHovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        if (viewportClicked)
            sceneViewportActive_ = true;

        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            sceneViewportActive_ = false;

        const ImVec2 availableSize = ImGui::GetContentRegionAvail();
        const VkExtent2D sceneExtent = leCore_.getRenderer().getSceneRenderTarget().getExtent();

        const float sceneAspect = static_cast<float>(sceneExtent.width) / static_cast<float>(sceneExtent.height);

        ImVec2 imageSize = availableSize;

        if (availableSize.x / availableSize.y > sceneAspect)
        {
            imageSize.x = availableSize.y * sceneAspect;
        }
        else
        {
            imageSize.y = availableSize.x / sceneAspect;
        }

        // Center the image in the Scene window.
        const float offsetX = (availableSize.x - imageSize.x) * 0.5f;
        const float offsetY = (availableSize.y - imageSize.y) * 0.5f;

        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offsetX, ImGui::GetCursorPosY() + offsetY));

        if (sceneTextureDescriptorSet_ != VK_NULL_HANDLE)
        {
            ImGui::Image(sceneTextureDescriptorSet_, imageSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        }

        ImGui::End();

        ImGui::Begin("Inspector");
        ImGui::Text("No entity selected");
        ImGui::End();

        ImGui::Begin("Hierarchy");
        ImGui::Text("Scene hierarchy");
        ImGui::End();

        ImGui::Begin("Console");
        ImGui::Text("Console output");
        ImGui::End();
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
    }

    void SeCore::renderEditor()
    {
    }
} // se