#include "vk/texture.hpp"
#include "vk/buffer.hpp"
#include "vk/singletimecommandbuffer.hpp"
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace democollection::vk
{
	TextureResources::TextureResources(const Vulkan& vulkan)
		: m_vulkan{vulkan}
		, m_image{VK_NULL_HANDLE}
		, m_view{VK_NULL_HANDLE}
		, m_memory{VK_NULL_HANDLE}
		, m_sampler{VK_NULL_HANDLE}
	{}

	TextureResources::~TextureResources()
	{
		SAFE_DESTROY(vkDestroySampler, m_sampler, m_vulkan.Device(), m_sampler, m_vulkan.Allocator());
		SAFE_DESTROY(vkDestroyImage, m_image, m_vulkan.Device(), m_image, m_vulkan.Allocator());
		SAFE_DESTROY(vkDestroyImageView, m_view, m_vulkan.Device(), m_view, m_vulkan.Allocator());
		SAFE_DESTROY(vkFreeMemory, m_memory, m_vulkan.Device(), m_memory, m_vulkan.Allocator());
	}

	void Texture::Init(const void* pixels, uint32_t width, uint32_t height)
	{
		const VkDeviceSize imageSize = 4ULL * width * height;
		const uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(static_cast<float>(std::max(width, height)))));

		const Buffer stagingBuffer(m_vulkan, Buffer::Type::Staging, imageSize, pixels);
		CreateImage(width, height, mipLevels);
		TransitionImageLayout(mipLevels);
		CopyBufferToImage(stagingBuffer.Get(), width, height);
		GenerateMipmaps(width, height, mipLevels);
		CreateImageView(mipLevels);
		CreateSampler(mipLevels);
	}

	void Texture::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;
		ThrowIfFailed(vkCreateImage(m_vulkan.Device(), &imageInfo, m_vulkan.Allocator(), &m_image));

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_vulkan.Device(), m_image, &memRequirements);
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = m_vulkan.FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		ThrowIfFailed(vkAllocateMemory(m_vulkan.Device(), &allocInfo, nullptr, &m_memory));

		vkBindImageMemory(m_vulkan.Device(), m_image, m_memory, 0);
	}

	void Texture::TransitionImageLayout(uint32_t mipLevels)
	{
		SingleTimeCommandBuffer cmdBuffer(m_vulkan);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(cmdBuffer.CommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		cmdBuffer.Submit();
	}

	void Texture::CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height)
	{
		SingleTimeCommandBuffer cmdBuffer(m_vulkan);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0};
		region.imageExtent = {width, height, 1 };
		vkCmdCopyBufferToImage(cmdBuffer.CommandBuffer(), buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		cmdBuffer.Submit();
	}

	void Texture::GenerateMipmaps(uint32_t width, uint32_t height, uint32_t mipLevels)
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(m_vulkan.Gpu().Device(), VK_FORMAT_R8G8B8A8_SRGB, &formatProperties);
		ThrowIfFalse(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);
		SingleTimeCommandBuffer cmdBuffer(m_vulkan);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = m_image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		int32_t mipWidth = width, mipHeight = height;

		for (uint32_t i = 1; i < mipLevels; ++i)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(cmdBuffer.CommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
					0, nullptr, 0, nullptr, 1, &barrier);

			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.mipLevel = i - 1;
			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight> 1) mipHeight /= 2;
			blit.dstOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.dstSubresource.mipLevel = i;

			vkCmdBlitImage(cmdBuffer.CommandBuffer(),
					m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(cmdBuffer.CommandBuffer(),
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr, 0, nullptr, 1, &barrier);
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmdBuffer.CommandBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr, 0, nullptr, 1, &barrier);

		cmdBuffer.Submit();
	}

	void Texture::CreateImageView(uint32_t mipLevels)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		ThrowIfFailed(vkCreateImageView(m_vulkan.Device(), &viewInfo, m_vulkan.Allocator(), &m_view));
	}

	void Texture::CreateSampler(uint32_t mipLevels)
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = m_vulkan.Gpu().Features().samplerAnisotropy;
		samplerInfo.maxAnisotropy = m_vulkan.Gpu().Properties().limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(mipLevels);
		ThrowIfFailed(vkCreateSampler(m_vulkan.Device(), &samplerInfo, m_vulkan.Allocator(), &m_sampler));
	}

	Texture::Texture(const Vulkan& vulkan, const char name[])
		: TextureResources(vulkan)
	{
		int width, height, channels;
		struct StbiPixels
		{
			stbi_uc* pixels;

			StbiPixels(stbi_uc* pixels)
				: pixels{pixels}
			{}
			~StbiPixels()
			{
				if (pixels)
					stbi_image_free(pixels);
			}
			inline operator void*() const { return pixels; }
			inline bool operator!() const { return !pixels; }
		};
		StbiPixels pixels = stbi_load(name, &width, &height, &channels, STBI_rgb_alpha);
		ThrowIfFalse(pixels);
		Init(pixels, width, height);
	}

	Texture::Texture(const Vulkan& vulkan, const void* pixels, uint32_t width, uint32_t height)
		: TextureResources(vulkan)
	{
		Init(pixels, width, height);
	}
}
