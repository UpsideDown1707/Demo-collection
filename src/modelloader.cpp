#include "modelloader.hpp"
#include "pmxloader.hpp"

namespace democollection
{
	ModelLoader::ModelLoader()
	{}

	void ModelLoader::MakeCube(const mth::float3& corner1, const mth::float3& corner2)
	{
		vertices = {
				// top
				vk::Vertex{mth::float3(corner1(0), corner2(1), corner1(2)), mth::float2(0.0f, 0.0f), mth::float3( 0.0f,  1.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner1(0), corner2(1), corner2(2)), mth::float2(0.0f, 1.0f), mth::float3( 0.0f,  1.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner2(1), corner2(2)), mth::float2(1.0f, 1.0f), mth::float3( 0.0f,  1.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner2(1), corner1(2)), mth::float2(1.0f, 0.0f), mth::float3( 0.0f,  1.0f,  0.0f), {}, {}},
				// bottom
				vk::Vertex{mth::float3(corner1(0), corner1(1), corner2(2)), mth::float2(0.0f, 0.0f), mth::float3( 0.0f, -1.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner1(0), corner1(1), corner1(2)), mth::float2(0.0f, 1.0f), mth::float3( 0.0f, -1.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner1(1), corner1(2)), mth::float2(1.0f, 1.0f), mth::float3( 0.0f, -1.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner1(1), corner2(2)), mth::float2(1.0f, 0.0f), mth::float3( 0.0f, -1.0f,  0.0f), {}, {}},
				// front
				vk::Vertex{mth::float3(corner1(0), corner1(1), corner1(2)), mth::float2(0.0f, 0.0f), mth::float3( 0.0f,  0.0f, -1.0f), {}, {}},
				vk::Vertex{mth::float3(corner1(0), corner2(1), corner1(2)), mth::float2(0.0f, 1.0f), mth::float3( 0.0f,  0.0f, -1.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner2(1), corner1(2)), mth::float2(1.0f, 1.0f), mth::float3( 0.0f,  0.0f, -1.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner1(1), corner1(2)), mth::float2(1.0f, 0.0f), mth::float3( 0.0f,  0.0f, -1.0f), {}, {}},
				// back
				vk::Vertex{mth::float3(corner2(0), corner1(1), corner2(2)), mth::float2(0.0f, 0.0f), mth::float3( 0.0f,  0.0f,  1.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner2(1), corner2(2)), mth::float2(0.0f, 1.0f), mth::float3( 0.0f,  0.0f,  1.0f), {}, {}},
				vk::Vertex{mth::float3(corner1(0), corner2(1), corner2(2)), mth::float2(1.0f, 1.0f), mth::float3( 0.0f,  0.0f,  1.0f), {}, {}},
				vk::Vertex{mth::float3(corner1(0), corner1(1), corner2(2)), mth::float2(1.0f, 0.0f), mth::float3( 0.0f,  0.0f,  1.0f), {}, {}},
				// left
				vk::Vertex{mth::float3(corner1(0), corner1(1), corner2(2)), mth::float2(0.0f, 0.0f), mth::float3(-1.0f,  0.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner1(0), corner2(1), corner2(2)), mth::float2(0.0f, 1.0f), mth::float3(-1.0f,  0.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner1(0), corner2(1), corner1(2)), mth::float2(1.0f, 1.0f), mth::float3(-1.0f,  0.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner1(0), corner1(1), corner1(2)), mth::float2(1.0f, 0.0f), mth::float3(-1.0f,  0.0f,  0.0f), {}, {}},
				// right
				vk::Vertex{mth::float3(corner2(0), corner1(1), corner1(2)), mth::float2(0.0f, 0.0f), mth::float3( 1.0f,  0.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner2(1), corner1(2)), mth::float2(0.0f, 1.0f), mth::float3( 1.0f,  0.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner2(1), corner2(2)), mth::float2(1.0f, 1.0f), mth::float3( 1.0f,  0.0f,  0.0f), {}, {}},
				vk::Vertex{mth::float3(corner2(0), corner1(1), corner2(2)), mth::float2(1.0f, 0.0f), mth::float3( 1.0f,  0.0f,  0.0f), {}, {}}
		};
		indices = {
				 0,  1,  2,  2,  3,  0,
				 4,  5,  6,  6,  7,  4,
				 8,  9, 10, 10, 11,  8,
				12, 13, 14, 14, 15, 12,
				16, 17, 18, 18, 19, 16,
				20, 21, 22, 22, 23, 20
		};
	}

	void ModelLoader::MakePlain(mth::float2 corner1, mth::float2 corner2, float plainY, mth::uint2 subdivisions)
	{
		size_t arrayLocationCounter;
		const mth::float2 size = corner2 - corner1;
		vertices.resize((subdivisions(0) + 1ULL) * (subdivisions(1) + 1ULL));
		indices.resize(6ULL * subdivisions(0) * subdivisions(1));

		arrayLocationCounter = 0;
		for (uint32_t y = 0; y < (1 + subdivisions(1)); ++y)
		{
			for (uint32_t x = 0; x < (1 + subdivisions(0)); ++x)
			{
				const mth::float2 scaling(
						static_cast<float>(x) / static_cast<float>(subdivisions(0)),
						static_cast<float>(y) / static_cast<float>(subdivisions(1))
						);
				vertices[arrayLocationCounter++] = vk::Vertex{
					mth::float3(corner1(0) + scaling(0) * size(0), plainY, corner1(1) + scaling(1) * size(1)),
					scaling,
					mth::float3(0.0f, 1.0f, 0.0f),
					{},
					{}
				};
			}
		}

		arrayLocationCounter = 0;
		for (uint32_t y = 0; y < subdivisions(1); ++y)
		{
			for (uint32_t x = 0; x < subdivisions(0); ++x)
			{
				indices[arrayLocationCounter++] = (subdivisions(0) + 1) * (y + 1) + (x + 0);
				indices[arrayLocationCounter++] = (subdivisions(0) + 1) * (y + 0) + (x + 1);
				indices[arrayLocationCounter++] = (subdivisions(0) + 1) * (y + 0) + (x + 0);
				indices[arrayLocationCounter++] = (subdivisions(0) + 1) * (y + 1) + (x + 1);
				indices[arrayLocationCounter++] = (subdivisions(0) + 1) * (y + 0) + (x + 1);
				indices[arrayLocationCounter++] = (subdivisions(0) + 1) * (y + 1) + (x + 0);
			}
		}
	}

	void ModelLoader::MakeUVSphere(const mth::float3& center, const mth::float3& radius, uint32_t latitudeCount, uint32_t longitudeCount)
	{
		if (latitudeCount < 3 || longitudeCount < 2)
		{
			Clear();
			return;
		}

		size_t arrayLocationCounter;
		const mth::float3 normalScaler = mth::Normalized(mth::float3(1.0f) / radius);
		vertices.resize((latitudeCount - 2ULL) * (longitudeCount + 1ULL) + 2ULL);
		indices.resize((latitudeCount - 2ULL) * longitudeCount * 6ULL);

		vertices[0] = vk::Vertex{
			center + mth::float3(0.0f, radius(1), 0.0f),
			mth::float2(0.5f, 0.0f),
			mth::float3(0.0f, 1.0f, 0.0f),
			{},
			{}
		};
		arrayLocationCounter = 1;
		for (uint32_t v = 1; v < latitudeCount - 1; ++v)
		{
			for (uint32_t u = 0; u < (longitudeCount + 1); ++u)
			{
				const mth::float2 scaling(
						static_cast<float>(u) / static_cast<float>(longitudeCount),
						static_cast<float>(v) / static_cast<float>(latitudeCount - 1)
						);
				const float a = scaling(1) * M_PIf;
				const float sina = std::sin(a);
				const float cosa = std::cos(a);
				const float b = scaling(0) * M_PIf * 2.0f;
				const float sinb = std::sin(b);
				const float cosb = std::cos(b);
				const mth::float3 normal(-1.0f * sinb * sina, cosa, cosb * sina);
				vertices[arrayLocationCounter++] = vk::Vertex{
					center + normal * radius,
					scaling,
					normal * normalScaler,
			{},
			{}
				};
			}
		}
		vertices[arrayLocationCounter] = vk::Vertex{
			center + mth::float3(0.0f, -radius(1), 0.0f),
			mth::float2(0.5f, 1.0f),
			mth::float3(0.0f, -1.0f, 0.0f),
			{},
			{}
		};

		arrayLocationCounter = 0;
		for (uint32_t u = 0; u < longitudeCount; ++u)
		{
			indices[arrayLocationCounter++] = 0;
			indices[arrayLocationCounter++] = 2 + u;
			indices[arrayLocationCounter++] = 1 + u;
		}
		for (uint32_t v = 1; v < (latitudeCount - 2); ++v)
		{
			for (uint32_t u = 0; u < longitudeCount; ++u)
			{
				indices[arrayLocationCounter++] = (longitudeCount + 1) * (v - 0) + (u + 0) + 1;
				indices[arrayLocationCounter++] = (longitudeCount + 1) * (v - 1) + (u + 0) + 1;
				indices[arrayLocationCounter++] = (longitudeCount + 1) * (v - 1) + (u + 1) + 1;
				indices[arrayLocationCounter++] = (longitudeCount + 1) * (v - 1) + (u + 1) + 1;
				indices[arrayLocationCounter++] = (longitudeCount + 1) * (v - 0) + (u + 1) + 1;
				indices[arrayLocationCounter++] = (longitudeCount + 1) * (v - 0) + (u + 0) + 1;
			}
		}
		uint32_t indexOffset = 1 + (latitudeCount - 3) * (longitudeCount + 1);
		const uint32_t lastIndex = vertices.size() - 1;
		for (uint32_t u = 0; u < longitudeCount; ++u)
		{
			indices[arrayLocationCounter++] = indexOffset;
			indices[arrayLocationCounter++] = ++indexOffset;
			indices[arrayLocationCounter++] = lastIndex;
		}
	}

	bool ModelLoader::LoadPmx(const char filename[])
	{
		PmxLoader loader(*this, filename);
		return loader.StatusInfo() == PmxLoader::Ok;
	}

	void ModelLoader::Clear()
	{
		vertices.clear();
		indices.clear();
	}

	void ModelLoader::Transform(const mth::float4x4& matrix)
	{
		mth::float3x3 normalMat = mth::Transpose(mth::Inverse(mth::float3x3(matrix)));
		normalMat /= mth::Determinant(normalMat);
		for (vk::Vertex& v : vertices)
		{
			v.position = mth::Transform(matrix, v.position);
			v.normal = normalMat * v.normal;
		}
	}
}
