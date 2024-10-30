#include "vk/uniformbuffer.hpp"

namespace democollection::vk
{
	UniformBuffer::UniformBuffer(const Vulkan& vulkan, VkDeviceSize size)
		: PerFrameBuffer(vulkan, Type::Uniform, size)
		, m_mappedData{}
	{
		ThrowIfFailed(vkMapMemory(m_vulkan.Device(), m_memory, 0, m_size * MAX_FRAMES_IN_FLIGHT, 0, &m_mappedData));
	}
}
