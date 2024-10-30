#pragma once

#include "modeltypes.hpp"

namespace democollection
{
	class PmxLoader
	{
	public:
		enum Status
		{
			Ok,
			FileNotFound,
			SignatureError,
			UnsupportedVersion,
			HeaderError,
			VertexError,
			MaterialError
		};

	private:
		struct Header
		{
			enum GlobalIndex
			{
				TextEncoding = 0,
				AdditionalVec4Count = 1,
				VertexIndexSize = 2,
				TextureIndexSize = 3,
				MaterialIndexSize = 4,
				BoneIndexSize = 5,
				MorphIndexSize = 6,
				RigidBodyIndexSize = 7
			};

			uint8_t signature[4];
			std::vector<uint8_t> globals;
			std::string jpModelName;
			std::string enModelName;
			std::string jpComments;
			std::string enComments;

		private:
			Status ReadSignature(std::ifstream& infile);
			Status ReadVersion(std::ifstream& infile);
			Status ReadGlobals(std::ifstream& infile);
			uint32_t ReadGlobalIndex(std::ifstream& infile, GlobalIndex idx) const;

		public:
			Status ReadHeader(std::ifstream& infile);

			std::string ReadText(std::ifstream& infile) const;
			uint32_t ReadVertexIndex(std::ifstream& infile) const;
			uint32_t ReadTextureIndex(std::ifstream& infile) const;
			uint32_t ReadMaterialIndex(std::ifstream& infile) const;
			uint32_t ReadBoneIndex(std::ifstream& infile) const;
			uint32_t ReadMorphIndex(std::ifstream& infile) const;
			uint32_t ReadRigidBodyIndex(std::ifstream& infile) const;
		};

		struct Material
		{
			enum DrawingFlags : uint8_t
			{
				DF_NoCull = 1 << 0,
				DF_GroundShadow = 1 << 1,
				DF_DrawShadow = 1 << 2,
				DF_ReceiveShadow = 1 << 3,
				DF_HasEdge = 1 << 4,
				DF_VertexColor = 1 << 5,
				DF_PointDrawing = 1 << 6,
				DF_LineDrawing = 1 << 7,
			};
			enum EnvironmentBlendMode : uint8_t
			{
				EBM_Disabled = 0,
				EBM_Multiply = 1,
				EBM_Additive = 2,
				EBM_AdditionanVec4 = 3,
				EBM_Last
			};
			enum ToonReference : uint8_t
			{
				TR_TextureReference = 0,
				TR_InternalReference = 1
			};

			std::string jpName;
			std::string enName;
			mth::float4 diffuseColor;
			mth::float3 specularColor;
			float specularStrength;
			mth::float3 ambientColor;
			uint8_t drawingFlags;
			mth::float4 edgeColor;
			float edgeScale;
			int textureIndex;
			int environmentIndex;
			uint8_t environmentBlendMode;
			uint8_t toonReference;
			int toonValue;
			std::string metaData;
			int surfaceCount;

			Status Read(std::ifstream& infile, const Header& header);
		};

	private:
		ModelData& m_data;
		std::ifstream m_infile;
		std::string m_baseFolder;
		Header m_header;
		std::vector<std::string> m_textureNames;
		std::vector<Material> m_materials;
		Status m_status;

	private:
		Status Load();
		Status LoadVertices();
		Status LoadIndices();
		Status LoadTextureNames();
		Status LoadMaterials();

	public:
		PmxLoader(ModelData& modelData, const char filename[]);
		Status StatusInfo() const { return m_status; }
	};
}
