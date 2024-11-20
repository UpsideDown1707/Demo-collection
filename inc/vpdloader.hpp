#pragma once

#include "common.hpp"
#include "mth/linalg.hpp"

namespace democollection
{
	class VpdLoader
	{
	public:
		enum Status
		{
			Ok,
			FileNotFound,
			BoneCountError,
			BoneListError,
			BoneValueError
		};

		struct Bone
		{
			std::string boneName;
			mth::float3 translation;
			mth::float4 rotation;
		};
	
	private:
		std::vector<Bone> m_bones;

	public:
		VpdLoader();
		VpdLoader(const char filename[]);
		Status Read(const char filename[]);

		inline const std::vector<Bone>& Bones() const { return m_bones; }
	};
}