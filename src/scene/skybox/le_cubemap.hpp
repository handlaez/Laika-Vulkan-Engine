#ifndef LE_CUBEMAP_HPP
#define LE_CUBEMAP_HPP

#include "le_device.hpp"

#include <stb_image.h>

#include <stdexcept>
#include <vector>
#include <array>

namespace le
{
    class LeCubemap
    {
    public:
        LeCubemap(
            LeDevice& device,
            const std::array<std::string, 6>& faces
        );

        ~LeCubemap();

        VkImageView getImageView() const;
        VkSampler getSampler() const;

    private:
        void createCubemapImage(uint32_t width, uint32_t height, VkFormat format);
        void createCubemapImageView();
        void createCubemapSampler();

        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

        LeDevice& device_;

        VkImage image_;
        VkDeviceMemory memory_;
        VkImageView imageView_;
        VkSampler sampler_;
    };
}

#endif