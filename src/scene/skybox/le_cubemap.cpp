#include "le_cubemap.hpp"

namespace le
{
	LeCubemap::LeCubemap(LeDevice& device, const std::array<std::string, 6>& faces) : device_(device)
	{
		int width = 0;
		int height = 0;
		int channels = 0;

		std::vector<unsigned char*> facePixels(6);

		stbi_set_flip_vertically_on_load(true);

		for (size_t i = 0; i < 6; i++)
		{
			facePixels[i] = stbi_load(
				faces[i].c_str(),
				&width,
				&height,
				&channels,
				STBI_rgb_alpha
			);

			if (!facePixels[i])
			{
				throw std::runtime_error("failed to load cubemap face!");
			}
		}

		VkDeviceSize layerSize = width * height * 4;
		VkDeviceSize imageSize = layerSize * 6;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		device_.createBuffer(
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		void* data;

		vkMapMemory(
			device_.device(),
			stagingBufferMemory,
			0,
			imageSize,
			0,
			&data
		);

		for (size_t i = 0; i < 6; i++)
		{
			memcpy(
				static_cast<char*>(data) + layerSize * i,
				facePixels[i],
				static_cast<size_t>(layerSize)
			);
		}

		vkUnmapMemory(device_.device(), stagingBufferMemory);

		createCubemapImage(width, height, VK_FORMAT_R8G8B8A8_SRGB);

		transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		device_.copyBufferToCubemap(stagingBuffer, image_, width, height);
		transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		createCubemapImageView();
		createCubemapSampler();

		vkDestroyBuffer(
			device_.device(),
			stagingBuffer,
			nullptr
		);

		vkFreeMemory(
			device_.device(),
			stagingBufferMemory,
			nullptr
		);

		for (auto pixels : facePixels)
		{
			stbi_image_free(pixels);
		}
	}

	LeCubemap::~LeCubemap()
	{
		vkDestroySampler(
			device_.device(),
			sampler_,
			nullptr
		);

		vkDestroyImageView(
			device_.device(),
			imageView_,
			nullptr
		);

		vkDestroyImage(
			device_.device(),
			image_,
			nullptr
		);

		vkFreeMemory(
			device_.device(),
			memory_,
			nullptr
		);
	}

	void LeCubemap::createCubemapImage(uint32_t width, uint32_t height, VkFormat format)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 6;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(
			device_.device(),
			&imageInfo,
			nullptr,
			&image_
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create cubemap image!");
		}

		VkMemoryRequirements memRequirements;

		vkGetImageMemoryRequirements(device_.device(), image_, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex =
			device_.findMemoryType(
				memRequirements.memoryTypeBits,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		if (vkAllocateMemory(
			device_.device(),
			&allocInfo,
			nullptr,
			&memory_
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate cubemap memory!");
		}

		vkBindImageMemory(
			device_.device(),
			image_,
			memory_,
			0
		);
	}

	void LeCubemap::createCubemapImageView()
	{
		VkImageViewCreateInfo viewInfo{};

		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image_;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;

		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 6;

		if (vkCreateImageView(
			device_.device(),
			&viewInfo,
			nullptr,
			&imageView_
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create cubemap image view!");
		}
	}

	void LeCubemap::createCubemapSampler()
	{
		VkSamplerCreateInfo samplerInfo{};

		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(
			device_.device(),
			&samplerInfo,
			nullptr,
			&sampler_
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create cubemap sampler!");
		}
	}

	void LeCubemap::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = device_.beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};

		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image_;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 6;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask =	VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::runtime_error("unsupported cubemap layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage,
			destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		device_.endSingleTimeCommands(commandBuffer);
	}

	VkImageView LeCubemap::getImageView() const
	{
		return imageView_;
	}

	VkSampler LeCubemap::getSampler() const
	{
		return sampler_;
	}
}