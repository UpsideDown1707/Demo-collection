#pragma once

#include "mth/position.hpp"

namespace democollection
{
	class Camera : public mth::Positionf
	{
		mth::float4x4 m_projection;
		float m_fov;
		float m_screenAspectRatio;
		float m_screenNear;
		float m_screenFar;

	private:
		void UpdateProjection();

	public:
		Camera();

		void UpdateScreenResolution(int width, int height);
		void UpdateFov(float fov);
		void UpdateScreenDepth(float near, float far);

		mth::float4x4 CameraMatrix() const;
		mth::float4x4 View() const;
		inline const mth::float4x4& Projection() const { return m_projection; }
	};
}
