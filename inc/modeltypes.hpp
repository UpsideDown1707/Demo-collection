#pragma once

#include "common.hpp"
#include "vk/types.hpp"

namespace democollection
{
	struct MaterialData
	{
		uint32_t firstIndex;
		uint32_t indexCount;
		vk::ModelBufferFs data;
		std::string textureName;
	};

	struct ModelData
	{
		std::vector<vk::Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<MaterialData> materials;
		std::vector<vk::Bone> skeleton;
	};
}
