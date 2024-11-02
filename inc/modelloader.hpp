#pragma once

#include "modeltypes.hpp"

namespace democollection
{
	class ModelLoader : private ModelData
	{
	public:
		ModelLoader();

		void MakeCube(const mth::float3& corner1, const mth::float3& corner2);
		void MakePlain(mth::float2 corner1, mth::float2 corner2, float plainY, mth::uint2 subdivisions);
		void MakeUVSphere(const mth::float3& center, const mth::float3& radius, uint32_t latitudeCount, uint32_t longitudeCount);

		bool LoadPmx(const char filename[]);

		void Clear();

		void Transform(const mth::float4x4& matrix);

		inline const std::vector<vk::Vertex>& Vertices() const { return vertices; }
		inline const std::vector<uint32_t>& Indices() const { return indices; }
		inline const std::vector<MaterialData>& Materials() const { return materials; }
		inline const std::vector<vk::Bone>& Skeleton() const { return skeleton; }
	};
}
