#pragma once

#include <vk/vulkan.hpp>

namespace democollection::vk
{
	class TextureResources
	{
		TextureResources(const TextureResources&) = delete;
		TextureResources(TextureResources&&) = delete;
		void operator=(const TextureResources&) = delete;
		void operator=( TextureResources&&) = delete;
	protected:
		const Vulkan& m_vulkan;
		VkImage m_image;
		VkImageView m_view;
		VkDeviceMemory m_memory;
		VkSampler m_sampler;

	protected:
		TextureResources(const Vulkan& vulkan);
		~TextureResources();
	};

	class Texture : private TextureResources
	{
	private:
		void Init(const void* pixels, uint32_t width, uint32_t height);
		void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels);
		void TransitionImageLayout(uint32_t mipLevels);
		void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
		void GenerateMipmaps(uint32_t width, uint32_t height, uint32_t mipLevels);
		void CreateImageView(uint32_t mipLevels);
		void CreateSampler(uint32_t mipLevels);

	public:
		Texture(const Vulkan& vulkan, const char name[]);
		Texture(const Vulkan& vulkan, const void* pixels, uint32_t width, uint32_t height);

		inline VkImageView ImageView() const { return m_view; }
		inline VkSampler Sampler() const { return m_sampler; }
	};
}
