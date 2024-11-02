#pragma once

#include "mth/linalg.hpp"

namespace democollection::vk
{
	struct Vertex
	{
		mth::float3 position;
		mth::float2 texcoord;
		mth::float3 normal;
		float boneWeights[4];
		uint32_t boneIndices[4];
	};

	struct Bone
	{
		mth::float4x4 toLocalTransform = mth::Identity<float, 4>();
		mth::float4x4 boneTransform = mth::Identity<float, 4>();
		mth::float4x4 toGlobalTransform = mth::Identity<float, 4>();
		Bone* parent = nullptr;
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

	struct ModelBufferFs
	{
		mth::float4 diffuseColor;
		mth::float3 specularColor;
		float specularPower;
		int padding[8];
	};
}
