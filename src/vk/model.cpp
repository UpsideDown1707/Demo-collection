#include "vk/model.hpp"
#include "image.hpp"

namespace democollection::vk
{
	Model::Model(Graphics& graphics,
			const UniformBuffer& sceneBufferVs,
			const UniformBuffer& sceneBufferFs,
			const ModelLoader& modelLoader)
		: m_graphics{graphics}
	{
		m_vsBuffer = std::make_unique<UniformBuffer>(graphics, sizeof(ModelBufferVs));

		m_mesh = std::make_unique<Mesh>(graphics,
				modelLoader.Vertices().data(), static_cast<uint32_t>(modelLoader.Vertices().size()),
				modelLoader.Indices().data(), static_cast<uint32_t>(modelLoader.Indices().size()));

		const std::vector<MaterialData>& materials = modelLoader.Materials();
		m_descriptorPool = std::make_unique<DescriptorPool>(graphics, materials.size());
		m_parts.resize(materials.size());
		for (size_t i = 0; i < materials.size(); ++i)
		{
			m_parts[i].firstIndex = materials[i].firstIndex;
			m_parts[i].indexCount = materials[i].indexCount;
			m_parts[i].fsBuffer = std::make_unique<UniformBuffer>(graphics, sizeof(ModelBufferFs));
			m_parts[i].texture = graphics.LoadTexture(materials[i].textureName);
			m_parts[i].descriptorSet = std::make_unique<DescriptorSet>(graphics, *m_descriptorPool, sceneBufferVs, *m_vsBuffer, sceneBufferFs, *m_parts[i].fsBuffer, *m_parts[i].texture);
		}
	}

	void Model::Render() const
	{
		m_mesh->Bind();
		for (const ModelPart& part : m_parts)
		{
			part.descriptorSet->Bind();
			m_mesh->Draw(part.firstIndex, part.indexCount);
		}
	}
}
