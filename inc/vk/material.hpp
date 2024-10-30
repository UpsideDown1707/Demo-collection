#pragma once

#include "types.hpp"
#include "descriptorset.hpp"
#include "texture.hpp"

namespace democollection::vk
{
	class Material
	{
		UniformBuffer m_materialData;
		Texture m_texture;
		DescriptorSet m_descriptionSet;

	public:
		Material(const Vulkan& vulkan);
	};
}
