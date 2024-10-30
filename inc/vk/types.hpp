#pragma once

#include "mth/linalg.hpp"

namespace democollection::vk
{
	struct Vertex
	{
		mth::float3 position;
		mth::float2 texcoord;
		mth::float3 normal;
	};

	struct SceneBufferVs
	{
		mth::float4x4 cameraMatrix;
	};

	struct SceneBufferFs
	{
		mth::float4 lightPosition;
		mth::float4 lightColor;
		int padding[8];
	};

	struct ModelBufferVs
	{
		mth::float4x4 bones[100];
	};

	struct ModelBufferFs
	{
		mth::float4 diffuseColor;
		mth::float3 specularColor;
		float specularPower;
		int padding[8];
	};
}
