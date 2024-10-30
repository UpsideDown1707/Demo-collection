#include <vk/buffer.hpp>

namespace democollection::vk
{
	Buffer::Buffer(const Vulkan& vulkan, Type type, VkDeviceSize size, const void* initialData)
		: BufferBase<1>(vulkan, type, size, initialData)
		{}

	PerFrameBuffer::PerFrameBuffer(const Vulkan& vulkan, Type type, VkDeviceSize size, const void* initialData)
		: BufferBase<MAX_FRAMES_IN_FLIGHT>(vulkan, type, size, initialData)
		{}
}
