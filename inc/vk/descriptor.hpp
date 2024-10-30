#pragma once

#include "uniformbuffer.hpp"
#include "texture.hpp"

namespace democollection::vk
{
	class DescriptorPoolResources
	{
	protected:
		const Vulkan& m_vulkan;
		VkDescriptorPool m_descriptorPool;
	protected:
		DescriptorPoolResources(const Vulkan& vulkan);
		~DescriptorPoolResources();
	};

	class DescriptorPool : private DescriptorPoolResources
	{
	public:
		DescriptorPool(const Vulkan& vulkan, uint32_t capacity);
		inline operator VkDescriptorPool() const { return m_descriptorPool; }
	};

	class DescriptorSetResources
	{
	protected:
		const Vulkan& m_vulkan;
		const VkDescriptorPool m_descriptorPool;
		VkDescriptorSet m_descriptorSets[MAX_FRAMES_IN_FLIGHT];
	protected:
		DescriptorSetResources(const Vulkan& vulkan, VkDescriptorPool descriptorPool);
		~DescriptorSetResources();
	};

	class DescriptorSet : private DescriptorSetResources
	{
	public:
		DescriptorSet(const Vulkan& vulkan,
				VkDescriptorPool descriptorPool,
				const UniformBuffer& sceneBufferVs,
				const UniformBuffer& modelBufferVs,
				const UniformBuffer& sceneBufferFs,
				const UniformBuffer& modelBufferFs,
				const Texture& texture);
		void Bind() const;
	};
}
