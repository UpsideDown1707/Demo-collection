#pragma once

#include <vk/descriptor.hpp>
#include "mesh.hpp"
#include "graphics.hpp"
#include "modelloader.hpp"

namespace democollection::vk
{
	class Model
	{
		struct ModelPart
		{
			uint32_t firstIndex;
			uint32_t indexCount;
			std::unique_ptr<UniformBuffer> fsBuffer;
			std::shared_ptr<Texture> texture;
			std::unique_ptr<DescriptorSet> descriptorSet;
		};

	private:
		Graphics& m_graphics;
		std::unique_ptr<UniformBuffer> m_vsBuffer;
		std::unique_ptr<Mesh> m_mesh;
		std::unique_ptr<DescriptorPool> m_descriptorPool;
		std::vector<ModelPart> m_parts;

	public:
		Model(Graphics& graphics,
				const UniformBuffer& sceneBufferVs,
				const UniformBuffer& sceneBufferFs,
				const ModelLoader& modelLoader);

		void Render() const;
	};
}
