#include "vk/model.hpp"
#include "image.hpp"
#include <iostream>

namespace democollection::vk
{
	mth::float4x4& Model::BoneTransforms(int index) const
	{
		return m_vsBuffer->Data<mth::float4x4>()[index];
	}

	Model::Model(Graphics& graphics,
			const UniformBuffer& sceneBufferVs,
			const UniformBuffer& sceneBufferFs,
			const ModelLoader& modelLoader)
		: m_graphics{graphics}
	{
		m_vsBuffer = std::make_unique<UniformBuffer>(graphics, sizeof(mth::float4x4) * modelLoader.Skeleton().size());

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

		m_skeleton = modelLoader.Skeleton();
	}

	void Model::ClearBoneTransforms()
	{
		for (vk::Bone& b : m_skeleton)
		{
			b.translation = mth::float3(0.0f, 0.0f, 0.0f);
			b.rotation = mth::float4(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	void Model::SetBoneTransform(const std::string& boneName, const mth::float3 translation, const mth::float4& rotation)
	{
		for (vk::Bone& b : m_skeleton)
		{
			if (0 == strcmp(b.name.c_str(), boneName.c_str()))
			{
				b.translation = translation;
				b.rotation = rotation;
			}
		}
	}

	void Model::UpdateInheritTransforms()
	{
		for (vk::Bone& b : m_skeleton)
		{
			if (b.inheritTranslationIdx > -1)
				b.translation += m_skeleton[b.inheritTranslationIdx].translation * b.inheritTranslationWeight;
			if (b.inheritRotationIdx > -1)
				b.rotation = mth::Slerp(b.rotation, m_skeleton[b.inheritRotationIdx].rotation, b.inheritRotationWeight);
		}
	}

	void Model::Update()
	{
		for (size_t i = 0; i < m_skeleton.size(); ++i)
		{
			Bone& b = m_skeleton[i];
			b.boneTransform = b.toParentTransform * (mth::Translation4x4(b.translation) * mth::RotationQuaternion4x4(b.rotation));
			if (b.parentIdx >= 0)
				b.boneTransform = m_skeleton[b.parentIdx].boneTransform * b.boneTransform;
			BoneTransforms(i) = mth::Transpose(b.boneTransform * b.toLocalTransform);
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
