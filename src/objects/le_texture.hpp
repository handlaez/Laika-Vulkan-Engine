#ifndef LE_TEXTURE_HPP
#define LE_TEXTURE_HPP

#include <string>
#include <stb_image.h>

#include "src/core/le_device.hpp"

namespace le {
	struct TextureDescriptor {
		VkDescriptorImageInfo imageInfo{};
	};

	class LeTexture {
	public:
		LeTexture(LeDevice& device, std::string path);
		~LeTexture();

		void setName(const std::string name);
		void setPath(const std::string path);
		const std::string& getName() const;

		VkImageView getImageView() const { return imageView; }

		VkDescriptorImageInfo getDescriptorInfo(VkSampler sampler) const;
	private:
		void createTextureImage(std::string path);
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void createTextureImageView();
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
			VkImageLayout newLayout);

		LeDevice& leDevice;

		std::string name_;
		std::string path_;

		VkImage textureImage;
		VkImageView imageView;
		VkDeviceMemory textureImageMemory;
	};
}

#endif