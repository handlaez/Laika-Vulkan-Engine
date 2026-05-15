#ifndef LE_RENDER_SYSTEM_MANAGER_RENDERER_HPP
#define LE_RENDER_SYSTEM_MANAGER_RENDERER_HPP

#include "le_renderer.hpp"
#include "basic_render_system.hpp"
#include "instanced_render_system.hpp"
#include "le_scene.hpp"
#include "le_texture.hpp"
#include "le_device.hpp"
#include "le_camera.hpp"
#include "le_swapchain.hpp"
#include "skybox_render_system.hpp"

#include <array>
#include <stdexcept>
#include <memory>

namespace le {
    struct LightingUBO
    {
        glm::vec4 lightColor;
        glm::vec4 lightDir;
        glm::vec4 cameraPos;

        int lightingEnabled;
        int texturesEnabled;

        alignas(8) glm::ivec2 padding;
    };

    class LeRenderSystemManager {
    public:
        LeRenderSystemManager(
            LeDevice& device,
            LeRenderer& renderer,
            LeResourceManager& resourceManager
        );
        ~LeRenderSystemManager();

        void sync(LeScene& scene);
        void render(LeScene& scene);
        
        void createFrameResources();
        void updateFrameUBO(uint32_t frameIndex, const LeCamera& camera);

        VkDescriptorSet getFrameDescriptorSet(uint32_t frameIndex) const;
        VkDescriptorSetLayout getFrameSetLayout() const { return frameSetLayout_; }
        VkDescriptorSetLayout getTextureSetLayout() const { return textureSetLayout_; }

        void updateLightingUBO(uint32_t frameIndex);

    private:
        void createDescriptorSetLayouts();

        LeDevice& device_;
        LeRenderer& renderer_;

        std::unique_ptr<BasicRenderSystem> basicRenderSystem;
        std::unique_ptr<InstancedRenderSystem> instancedRenderSystem;
        std::unique_ptr<SkyboxRenderSystem> skyboxRenderSystem;

        VkDescriptorSetLayout frameSetLayout_{ VK_NULL_HANDLE };
        VkDescriptorSetLayout textureSetLayout_{ VK_NULL_HANDLE };

        // frame buffer
        std::vector<VkBuffer> frameUniformBuffers_;
        std::vector<VkDeviceMemory> frameUniformBuffersMemory_;
        std::vector<void*> frameUniformBuffersMapped_;

        // lighting buffer
        std::vector<VkBuffer> lightingUniformBuffers_;
        std::vector<VkDeviceMemory> lightingUniformBuffersMemory_;
        std::vector<void*> lightingUniformBuffersMapped_;

        VkDescriptorPool frameDescriptorPool_{ VK_NULL_HANDLE };
        std::vector<VkDescriptorSet> frameDescriptorSets_;

        LeResourceManager& resourceManager_;
    };

}

#endif