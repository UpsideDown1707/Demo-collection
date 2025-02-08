#include "pmxloader.hpp"

namespace democollection
{
#define READ(primitive) infile.read((char*)&(primitive), sizeof(primitive))
#define READ_SOME(address, length) infile.read((char*)(address), length)

#define RETURN_IF_ERROR(status) do{PmxLoader::Status st=status;if(st!=PmxLoader::Status::Ok)return st;}while(false)

	PmxLoader::Status PmxLoader::Header::ReadSignature(std::ifstream& infile)
	{
		READ_SOME(signature, sizeof(signature));
		if (0 != memcmp(signature, "PMX ", sizeof(signature)))
			return PmxLoader::Status::SignatureError;
		return PmxLoader::Status::Ok;
	}

	PmxLoader::Status PmxLoader::Header::ReadVersion(std::ifstream& infile)
	{
		float ver = 0.0f;
		READ(ver);
		if (2.0f != ver)
			return PmxLoader::Status::UnsupportedVersion;
		return PmxLoader::Status::Ok;
	}

	PmxLoader::Status PmxLoader::Header::ReadGlobals(std::ifstream& infile)
	{
		uint8_t globalsCount = 0;
		READ(globalsCount);
		globals.resize(std::min<size_t>(8, globalsCount));
		globals[TextEncoding] = 0;
		globals[AdditionalVec4Count] = 0;
		globals[VertexIndexSize] = 4;
		globals[TextureIndexSize] = 4;
		globals[MaterialIndexSize] = 4;
		globals[BoneIndexSize] = 4;
		globals[MorphIndexSize] = 4;
		globals[RigidBodyIndexSize] = 4;
		READ_SOME(globals.data(), globals.size() * sizeof(globals[0]));

		if (globals[TextEncoding] > 1)
			return PmxLoader::Status::HeaderError;
		if (globals[AdditionalVec4Count] > 4)
			return PmxLoader::Status::HeaderError;
		for (int i = VertexIndexSize; i <= RigidBodyIndexSize; ++i)
		{
			uint8_t var = globals[i];
			if (var != 1 && var != 2 && var != 4)
				return PmxLoader::Status::HeaderError;
		}
		return PmxLoader::Status::Ok;
	}

	int PmxLoader::Header::ReadGlobalIndex(std::ifstream& infile, GlobalIndex idx) const
	{
		int i = 0;
		READ_SOME(&i, globals[idx]);
		return i;
	}

	PmxLoader::Status PmxLoader::Header::ReadHeader(std::ifstream& infile)
	{
		RETURN_IF_ERROR(ReadSignature(infile));
		RETURN_IF_ERROR(ReadVersion(infile));
		RETURN_IF_ERROR(ReadGlobals(infile));
		jpModelName = ReadText(infile);
		enModelName = ReadText(infile);
		jpComments = ReadText(infile);
		enComments = ReadText(infile);
		return PmxLoader::Status::Ok;
	}

	std::string PmxLoader::Header::ReadText(std::ifstream& infile) const
	{
		int length = 0;
		READ(length);
		if (globals[TextEncoding] == 0)
		{
			std::u16string text;
			text.resize(length / 2 + 1);
			infile.read(reinterpret_cast<char*>(&text[0]), length);
			return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().to_bytes(text);
		}
		else
		{
			std::string text;
			text.resize(length + 1);
			infile.read(reinterpret_cast<char*>(&text[0]), length);
			return text;
		}
	}
	int PmxLoader::Header::ReadVertexIndex(std::ifstream& infile) const { return ReadGlobalIndex(infile, VertexIndexSize); }
	int PmxLoader::Header::ReadTextureIndex(std::ifstream& infile) const { return ReadGlobalIndex(infile, TextureIndexSize); }
	int PmxLoader::Header::ReadMaterialIndex(std::ifstream& infile) const { return ReadGlobalIndex(infile, MaterialIndexSize); }
	int PmxLoader::Header::ReadBoneIndex(std::ifstream& infile) const
	{
		const int leadingZeros = 32 - (globals[BoneIndexSize] * 8);
		return ReadGlobalIndex(infile, BoneIndexSize) << leadingZeros >> leadingZeros;
	}
	int PmxLoader::Header::ReadMorphIndex(std::ifstream& infile) const { return ReadGlobalIndex(infile, MorphIndexSize); }
	int PmxLoader::Header::ReadRigidBodyIndex(std::ifstream& infile) const { return ReadGlobalIndex(infile, RigidBodyIndexSize); }

	PmxLoader::Status PmxLoader::Material::Read(std::ifstream& infile, const PmxLoader::Header& header)
	{
		jpName = header.ReadText(infile);
		enName = header.ReadText(infile);
		READ(diffuseColor);
		READ(specularColor);
		READ(specularStrength);
		READ(ambientColor);
		READ(drawingFlags);
		READ(edgeColor);
		READ(edgeScale);
		textureIndex = header.ReadTextureIndex(infile);
		environmentIndex = header.ReadTextureIndex(infile);
		READ(environmentBlendMode);
		if (environmentBlendMode >= EBM_Last)
			return PmxLoader::Status::MaterialError;
		READ(toonReference);
		switch (toonReference)
		{
		case TR_TextureReference:
			toonValue = header.ReadTextureIndex(infile);
			break;
		case TR_InternalReference:
			toonValue = 0;
			READ_SOME(&toonValue, sizeof(uint8_t));
			break;
		default:
			return PmxLoader::Status::MaterialError;
		}
		metaData = header.ReadText(infile);
		READ(surfaceCount);
		return PmxLoader::Status::Ok;
	}

	PmxLoader::Status PmxLoader::Bone::Read(std::ifstream& infile, const Header& header)
	{
		jpName = header.ReadText(infile);
		enName = header.ReadText(infile);
		READ(position);
		parentIndex = header.ReadBoneIndex(infile);
		READ(layer);
		READ(flags);
		if (flags & BoneFlags::IndexedTailPosition)
			tailPosition.boneIndex = header.ReadBoneIndex(infile);
		else
			READ(tailPosition.position);
		if (flags & (BoneFlags::InheritRotation | BoneFlags::InheritTranslation))
		{
			inheritBone.parentIndex = header.ReadBoneIndex(infile);
			READ(inheritBone.influenceWeight);
		}
		if (flags & BoneFlags::FixedAxis)
			READ(fixedAxis);
		if (flags & BoneFlags::LocalCoordinate)
			READ(localCoordinate);
		if (flags & BoneFlags::ExternalParentDeform)
			externalParent.parentIndex = header.ReadBoneIndex(infile);
		if (flags & BoneFlags::InverseKinematics)
		{
			inverseKinematics.targetIndex = header.ReadBoneIndex(infile);
			READ(inverseKinematics.loopCount);
			READ(inverseKinematics.limitRadian);
			uint32_t linkCount = 0;
			READ(linkCount);
			inverseKinematics.ikLinks.resize(linkCount);
			for (IkLinks& link : inverseKinematics.ikLinks)
			{
				link.boneIndex = header.ReadBoneIndex(infile);
				READ(link.hasLimits);
				if (link.hasLimits)
					READ(link.limits);
			}
		}

		return PmxLoader::Status::Ok;
	}

	PmxLoader::Status PmxLoader::Load()
	{
		RETURN_IF_ERROR(m_header.ReadHeader(m_infile));
		RETURN_IF_ERROR(LoadVertices());
		RETURN_IF_ERROR(LoadIndices());
		RETURN_IF_ERROR(LoadTextureNames());
		RETURN_IF_ERROR(LoadMaterials());
		RETURN_IF_ERROR(LoadBones());
		return Ok;
	}

	PmxLoader::Status PmxLoader::LoadVertices()
	{
		std::ifstream& infile = m_infile;
		uint32_t vertexCount = 0;
		READ(vertexCount);
		m_data.vertices.resize(vertexCount);
		for (vk::Vertex& v : m_data.vertices)
		{
			READ(v.position);
			READ(v.normal);
			READ(v.texcoord);
			if (uint8_t cnt = m_header.globals[Header::AdditionalVec4Count])
				infile.ignore(sizeof(mth::float4) * cnt);
			RETURN_IF_ERROR(LoadVertexBoneData(v));
			infile.ignore(sizeof(float));	// Edge scale
		}
		return Ok;
	}

	PmxLoader::Status PmxLoader::LoadVertexBoneData(vk::Vertex& vertex)
	{
		std::ifstream& infile = m_infile;
		uint8_t boneIndexSize = m_header.globals[Header::BoneIndexSize];
		uint8_t deformType;
		READ(deformType);
		switch (deformType)
		{
		case 0:
			READ_SOME(&vertex.boneIndices[0], boneIndexSize);
			vertex.boneWeights[0] = 1.0f;
			break;
		case 1:
			READ_SOME(&vertex.boneIndices[0], boneIndexSize);
			READ_SOME(&vertex.boneIndices[1], boneIndexSize);
			READ_SOME(&vertex.boneWeights[0], sizeof(float));
			vertex.boneWeights[1] = 1.0f - vertex.boneWeights[0];
			break;
		case 2:
			READ_SOME(&vertex.boneIndices[0], boneIndexSize);
			READ_SOME(&vertex.boneIndices[1], boneIndexSize);
			READ_SOME(&vertex.boneIndices[2], boneIndexSize);
			READ_SOME(&vertex.boneIndices[3], boneIndexSize);
			READ_SOME(vertex.boneWeights, sizeof(vertex.boneWeights));
			break;
		case 3:
			READ_SOME(&vertex.boneIndices[0], boneIndexSize);
			READ_SOME(&vertex.boneIndices[1], boneIndexSize);
			READ_SOME(&vertex.boneWeights[0], sizeof(float));
			vertex.boneWeights[1] = 1.0f - vertex.boneWeights[0];
			infile.ignore(sizeof(mth::float3) * 3);
			break;
		case 4:
			READ_SOME(&vertex.boneIndices[0], boneIndexSize);
			READ_SOME(&vertex.boneIndices[1], boneIndexSize);
			READ_SOME(&vertex.boneIndices[2], boneIndexSize);
			READ_SOME(&vertex.boneIndices[3], boneIndexSize);
			READ_SOME(vertex.boneWeights, sizeof(vertex.boneWeights));
			break;
		default:
			return VertexError;
		}
		return Ok;
	}

	PmxLoader::Status PmxLoader::LoadIndices()
	{
		std::ifstream& infile = m_infile;
		uint32_t indexCount = 0;
		READ(indexCount);
		m_data.indices.resize(indexCount);
		for (uint32_t& i : m_data.indices)
			i = m_header.ReadVertexIndex(infile);
		return Ok;
	}

	PmxLoader::Status PmxLoader::LoadTextureNames()
	{
		std::ifstream& infile = m_infile;
		uint32_t textureCount = 0;
		READ(textureCount);
		m_textureNames.reserve(textureCount);
		for (uint32_t i = 0; i < textureCount; ++i)
		{
			std::string tex = m_baseFolder + m_header.ReadText(infile);
			std::replace(tex.begin(), tex.end(), '\\', '/');
			m_textureNames.emplace_back(std::move(tex));
		}
		return Ok;
	}

	PmxLoader::Status PmxLoader::LoadMaterials()
	{
		std::ifstream& infile = m_infile;
		uint32_t materialCount = 0;
		READ(materialCount);
		m_materials.resize(materialCount);
		m_data.materials.resize(materialCount);
		uint32_t runningIndex = 0;
		for (uint32_t i = 0; i < materialCount; ++i)
		{
			RETURN_IF_ERROR(m_materials[i].Read(infile, m_header));
			m_data.materials[i].firstIndex = runningIndex;
			m_data.materials[i].indexCount = m_materials[i].surfaceCount;
			runningIndex += m_materials[i].surfaceCount;
			m_data.materials[i].data.diffuseColor = m_materials[i].diffuseColor;
			m_data.materials[i].data.specularColor = m_materials[i].specularColor;
			m_data.materials[i].data.specularPower = m_materials[i].specularStrength;
			m_data.materials[i].textureName = m_textureNames[m_materials[i].textureIndex];
		}
		return Ok;
	}

	PmxLoader::Status PmxLoader::LoadBones()
	{
		std::ifstream& infile = m_infile;
		uint32_t boneCount = 0;
		READ(boneCount);
		m_bones.resize(boneCount);
		for (Bone& bone : m_bones)
			RETURN_IF_ERROR(bone.Read(infile, m_header));
		
		m_data.skeleton.resize(boneCount);
		for (uint32_t i = 0; i < boneCount; ++i)
		{
			m_data.skeleton[i].name = m_bones[i].jpName;
			m_data.skeleton[i].toLocalTransform = mth::TranslationInv4x4(m_bones[i].position);
			m_data.skeleton[i].parentIdx = m_bones[i].parentIndex;
			if (m_bones[i].flags & Bone::BoneFlags::InheritTranslation)
			{
				m_data.skeleton[i].inheritTranslationIdx = m_bones[i].inheritBone.parentIndex;
				m_data.skeleton[i].inheritTranslationWeight = m_bones[i].inheritBone.influenceWeight;
			}
			if (m_bones[i].flags & Bone::BoneFlags::InheritRotation)
			{
				m_data.skeleton[i].inheritRotationIdx = m_bones[i].inheritBone.parentIndex;
				m_data.skeleton[i].inheritRotationWeight = m_bones[i].inheritBone.influenceWeight;
			}

			m_data.skeleton[i].translation = mth::float3(0.0f, 0.0f, 0.0f);
			m_data.skeleton[i].rotation = mth::float4(0.0f, 0.0f, 0.0f, 1.0f);
			if (m_bones[i].parentIndex < 0)
				m_data.skeleton[i].toParentTransform = mth::Translation4x4(m_bones[i].position);
			else
				m_data.skeleton[i].toParentTransform = mth::Translation4x4(m_bones[i].position - m_bones[m_bones[i].parentIndex].position);
		}

		return Ok;
	}

	PmxLoader::PmxLoader(ModelData& modelData, const char filename[])
		: m_data{modelData}
		, m_infile(filename, std::ios::binary)
		, m_baseFolder{GetFolderName(filename)}
		, m_status{m_infile.is_open() ? Load() : FileNotFound}
	{}
}
