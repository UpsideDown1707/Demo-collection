#include "vk/descriptor.hpp"

namespace democollection::vk
{
	DescriptorPoolResources::DescriptorPoolResources(const Vulkan& vulkan)
		: m_vulkan{vulkan}
		, m_descriptorPool{VK_NULL_HANDLE}
	{}

	DescriptorPoolResources::~DescriptorPoolResources()
	{
		SAFE_DESTROY(vkDestroyDescriptorPool, m_descriptorPool, m_vulkan.Device(), m_descriptorPool, m_vulkan.Allocator());
	}

	DescriptorPool::DescriptorPool(const Vulkan& vulkan, uint32_t capacity)
		: DescriptorPoolResources{vulkan}
	{
		VkDescriptorPoolSize poolSizes[5]{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT * capacity;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT * capacity;
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[2].descriptorCount = MAX_FRAMES_IN_FLIGHT * capacity;
		poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[3].descriptorCount = MAX_FRAMES_IN_FLIGHT * capacity;
		poolSizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[4].descriptorCount = MAX_FRAMES_IN_FLIGHT * capacity;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = ARRAY_SIZE(poolSizes);
		poolInfo.pPoolSizes = poolSizes;
		poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT * capacity;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		ThrowIfFailed(vkCreateDescriptorPool(m_vulkan.Device(), &poolInfo, m_vulkan.Allocator(), &m_descriptorPool));
	}

	DescriptorSetResources::DescriptorSetResources(const Vulkan& vulkan, VkDescriptorPool descriptorPool)
		: m_vulkan{vulkan}
		, m_descriptorPool{descriptorPool}
		, m_descriptorSets{}
	{
		for (VkDescriptorSet& ds : m_descriptorSets)
			ds = VK_NULL_HANDLE;
	}

	DescriptorSetResources::~DescriptorSetResources()
	{
		if (m_descriptorSets[0])
		{
			vkFreeDescriptorSets(m_vulkan.Device(), m_descriptorPool, MAX_FRAMES_IN_FLIGHT, m_descriptorSets);
			for (VkDescriptorSet& ds : m_descriptorSets)
				ds = VK_NULL_HANDLE;
		}
	}

	DescriptorSet::DescriptorSet(const Vulkan& vulkan,
			VkDescriptorPool descriptorPool,
			const UniformBuffer& sceneBufferVs,
			const UniformBuffer& modelBufferVs,
			const UniformBuffer& sceneBufferFs,
			const UniformBuffer& modelBufferFs,
			const Texture& texture)
		: DescriptorSetResources(vulkan, descriptorPool)
	{
		std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> descriptorSetLayouts;
		descriptorSetLayouts.fill(m_vulkan.DescriptorSetLayout());

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
		allocInfo.pSetLayouts = descriptorSetLayouts.data();
		ThrowIfFailed(vkAllocateDescriptorSets(m_vulkan.Device(), &allocInfo, m_descriptorSets));

		VkWriteDescriptorSet descriptorWrites[5]{};

		VkDescriptorBufferInfo sceneBufferVsInfo{};
		sceneBufferVsInfo.offset = 0;
		sceneBufferVsInfo.range = sceneBufferVs.Size();
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &sceneBufferVsInfo;
		descriptorWrites[0].pImageInfo = nullptr;
		descriptorWrites[0].pTexelBufferView = nullptr;

		VkDescriptorBufferInfo modelBufferVsInfo{};
		modelBufferVsInfo.offset = 0;
		modelBufferVsInfo.range = modelBufferVs.Size();
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &modelBufferVsInfo;
		descriptorWrites[1].pImageInfo = nullptr;
		descriptorWrites[1].pTexelBufferView = nullptr;

		VkDescriptorBufferInfo sceneBufferFsInfo{};
		sceneBufferFsInfo.offset = 0;
		sceneBufferFsInfo.range = sceneBufferFs.Size();
		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pBufferInfo = &sceneBufferFsInfo;
		descriptorWrites[2].pImageInfo = nullptr;
		descriptorWrites[2].pTexelBufferView = nullptr;

		VkDescriptorBufferInfo modelBufferFsInfo{};
		modelBufferFsInfo.offset = 0;
		modelBufferFsInfo.range = modelBufferFs.Size();
		descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[3].dstBinding = 3;
		descriptorWrites[3].dstArrayElement = 0;
		descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[3].descriptorCount = 1;
		descriptorWrites[3].pBufferInfo = &modelBufferFsInfo;
		descriptorWrites[3].pImageInfo = nullptr;
		descriptorWrites[3].pTexelBufferView = nullptr;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture.ImageView();
		imageInfo.sampler = texture.Sampler();
		descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[4].dstBinding = 4;
		descriptorWrites[4].dstArrayElement = 0;
		descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[4].descriptorCount = 1;
		descriptorWrites[4].pBufferInfo = nullptr;
		descriptorWrites[4].pImageInfo = &imageInfo;
		descriptorWrites[4].pTexelBufferView = nullptr;

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			sceneBufferVsInfo.buffer = sceneBufferVs.Get(i);
			modelBufferVsInfo.buffer = modelBufferVs.Get(i);
			sceneBufferFsInfo.buffer = sceneBufferFs.Get(i);
			modelBufferFsInfo.buffer = modelBufferFs.Get(i);
			descriptorWrites[0].dstSet = m_descriptorSets[i];
			descriptorWrites[1].dstSet = m_descriptorSets[i];
			descriptorWrites[2].dstSet = m_descriptorSets[i];
			descriptorWrites[3].dstSet = m_descriptorSets[i];
			descriptorWrites[4].dstSet = m_descriptorSets[i];
			vkUpdateDescriptorSets(m_vulkan.Device(), ARRAY_SIZE(descriptorWrites), descriptorWrites, 0, nullptr);
		}
	}

	void DescriptorSet::Bind() const
	{
		vkCmdBindDescriptorSets(m_vulkan.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_vulkan.PipelineLayout(), 0, 1, &m_descriptorSets[m_vulkan.CurrentFrame()], 0, nullptr);
	}
}
