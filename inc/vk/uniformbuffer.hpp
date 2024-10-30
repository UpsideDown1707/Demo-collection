#include "vk/buffer.hpp"

namespace democollection::vk
{
	class UniformBuffer : public PerFrameBuffer
	{
		void* m_mappedData;

	public:
		UniformBuffer(const Vulkan& vulkan, VkDeviceSize size);

		template <typename T = void*>
		inline T* Data() const
		{
			return reinterpret_cast<T*>(
					reinterpret_cast<uint8_t*>(m_mappedData) + m_size * m_vulkan.CurrentFrame()
					);
		}
	};
}
