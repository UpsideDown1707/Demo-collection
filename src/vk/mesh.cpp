#include "vk/mesh.hpp"


namespace democollection::vk
{
	Mesh::Mesh(const Vulkan& vulkan, const Vertex vertices[], uint32_t vertexCount, const uint32_t indices[], uint32_t indexCount)
		: m_vulkan{vulkan}
		,m_vertexBuffer(vulkan, Buffer::Type::Vertex, sizeof(Vertex) * vertexCount)
		, m_indexBuffer(vulkan, Buffer::Type::Index, sizeof(uint32_t) * indexCount)
	{
		m_vertexBuffer.CopyDataFrom(Buffer(vulkan, Buffer::Type::Staging, sizeof(Vertex) * vertexCount, vertices));
		m_indexBuffer.CopyDataFrom(Buffer(vulkan, Buffer::Type::Staging, sizeof(uint32_t) * indexCount, indices));
	}

	void Mesh::Bind() const
	{
		VkBuffer vertexBuffer = m_vertexBuffer.Get();
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(m_vulkan.CommandBuffer(), 0, 1, &vertexBuffer, &offset);
		vkCmdBindIndexBuffer(m_vulkan.CommandBuffer(), m_indexBuffer.Get(), 0, VK_INDEX_TYPE_UINT32);
	}

	void Mesh::Draw() const
	{
		Draw(0, m_indexBuffer.Size() / sizeof(uint32_t));
	}

	void Mesh::Draw(uint32_t first, uint32_t count) const
	{
		vkCmdDrawIndexed(m_vulkan.CommandBuffer(), count, 1, first, 0, 0);
	}
}
