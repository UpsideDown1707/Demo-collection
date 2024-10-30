#pragma once

#include "vk/singletimecommandbuffer.hpp"

namespace democollection::vk
{
	template <uint32_t C>
	class BufferResources
	{
		BufferResources(const BufferResources&) = delete;
		BufferResources(BufferResources&&) = delete;
		void operator=(const BufferResources&) = delete;
		void operator=(BufferResources&&) = delete;

	protected:
		const Vulkan& m_vulkan;
		VkBuffer m_buffers[C];
		VkDeviceMemory m_memory;

	protected:
		BufferResources(const Vulkan& vulkan)
			: m_vulkan{vulkan}
			, m_buffers{}
			, m_memory{VK_NULL_HANDLE}
		{
			for (VkBuffer& b : m_buffers)
				b = VK_NULL_HANDLE;
		}
		~BufferResources()
		{
			for (VkBuffer& b : m_buffers)
				SAFE_DESTROY(vkDestroyBuffer, b, m_vulkan.Device(), b, m_vulkan.Allocator());
			SAFE_DESTROY(vkFreeMemory, m_memory, m_vulkan.Device(), m_memory, m_vulkan.Allocator());
		}
	};

	template <uint32_t C>
	class BufferBase : protected BufferResources<C>
	{
	public:
		enum class Type
		{
			Staging,
			Vertex,
			Index,
			Uniform
		};

	protected:
		VkDeviceSize m_size;

	protected:
		BufferBase(const Vulkan& vulkan, Type type, VkDeviceSize size, const void* initialData)
			: BufferResources<C>(vulkan)
			, m_size{size}
		{
			VkBufferUsageFlags usage;
			VkMemoryPropertyFlags properties;

			switch (type) {
				case Type::Staging:
					usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
					properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
					break;
				case Type::Vertex:
					usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
					properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
					break;
				case Type::Index:
					usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
					properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
					break;
				case Type::Uniform:
					usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
					properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
					break;
				default:
					Throw("Unsupported buffer type");
			}

			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = m_size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			for (VkBuffer& b : BufferResources<C>::m_buffers)
				ThrowIfFailed(vkCreateBuffer(BufferResources<C>::m_vulkan.Device(), &bufferInfo, BufferResources<C>::m_vulkan.Allocator(), &b));

			VkMemoryRequirements memRequirements{};
			vkGetBufferMemoryRequirements(BufferResources<C>::m_vulkan.Device(), BufferResources<C>::m_buffers[0], &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size * C;
			allocInfo.memoryTypeIndex = BufferResources<C>::m_vulkan.FindMemoryType(memRequirements.memoryTypeBits, properties);
			ThrowIfFailed(vkAllocateMemory(BufferResources<C>::m_vulkan.Device(), &allocInfo, BufferResources<C>::m_vulkan.Allocator(), &(BufferResources<C>::m_memory)));

			for (uint32_t i = 0; i < C; ++i)
				ThrowIfFailed(vkBindBufferMemory(BufferResources<C>::m_vulkan.Device(), BufferResources<C>::m_buffers[i], BufferResources<C>::m_memory, i * m_size));

			if (initialData)
			{
				uint8_t* data;
				ThrowIfFailed(vkMapMemory(BufferResources<C>::m_vulkan.Device(), BufferResources<C>::m_memory, 0, m_size * C, 0, reinterpret_cast<void**>(&data)));
				for (uint32_t i = 0; i < C; ++i)
					memcpy(data + (i * m_size), initialData, m_size);
				vkUnmapMemory(BufferResources<C>::m_vulkan.Device(), BufferResources<C>::m_memory);
			}
		}

	public:
		void CopyDataFrom(const BufferBase<C>& src) const
		{
			SingleTimeCommandBuffer cmdBuffer(BufferResources<C>::m_vulkan);

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = m_size;
			for (uint32_t i = 0; i < C; ++i)
				vkCmdCopyBuffer(cmdBuffer.CommandBuffer(), src.m_buffers[i], BufferResources<C>::m_buffers[i], 1, &copyRegion);

			cmdBuffer.Submit();
		}
		inline VkDeviceSize Size() const { return m_size; }
	};

	class Buffer : public BufferBase<1>
	{
	public:
		Buffer(const Vulkan& vulkan, Type type, VkDeviceSize size, const void* initialData = nullptr);
		inline VkBuffer Get() const { return m_buffers[0]; }
	};

	class PerFrameBuffer : public BufferBase<MAX_FRAMES_IN_FLIGHT>
	{
	public:
		PerFrameBuffer(const Vulkan& vulkan, Type type, VkDeviceSize size, const void* initialData = nullptr);
		inline VkBuffer Get() const { return m_buffers[m_vulkan.CurrentFrame()]; }
		inline VkBuffer Get(uint32_t index) const { return m_buffers[index]; }
	};
}
