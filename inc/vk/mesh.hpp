#pragma once

#include "buffer.hpp"
#include "types.hpp"

namespace democollection::vk
{
	class Mesh
	{
		const Vulkan& m_vulkan;
		Buffer m_vertexBuffer;
		Buffer m_indexBuffer;

	public:
		Mesh(const Vulkan& vulkan, const Vertex vertices[], uint32_t vertexCount, const uint32_t indices[], uint32_t indexCount);

		void Bind() const;
		void Draw() const;
		void Draw(uint32_t first, uint32_t count) const;
	};
}
