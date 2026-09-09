#include "src/editor/se_core.hpp"

#include "src/demo_app.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <array>
#include <stdexcept>

namespace se {

    se::SeCore::SeCore() : leCore_{}, currentScene_ { leCore_.getDevice(), leCore_.getResources() }
    {
        initImGui();
    }

    se::SeCore::~SeCore()
    {
        shutdownImGui();
    }

    void se::SeCore::run()
    {
        DemoApp demoApp;
        demoApp.onStart(currentScene_);

        while (!leCore_.getWindow().shouldClose())
        {
            leCore_.beginFrame();

            demoApp.onUpdate(currentScene_, leCore_.getFrameInfo());
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
        initInfo.DescriptorPoolSize = 1000;

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
    }

    void SeCore::shutdownImGui()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void se::SeCore::updateEditor()
    {
    }

    void se::SeCore::renderEditor()
    {
    }
} // se