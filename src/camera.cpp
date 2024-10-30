#include "camera.hpp"

namespace democollection
{
	void Camera::UpdateProjection()
	{
		m_projection = mth::PerspectiveFOV(m_fov, m_screenAspectRatio, m_screenNear, m_screenFar);
	}

	Camera::Camera()
		: m_fov{M_PI_4}
		, m_screenAspectRatio{1.0f}
		, m_screenNear{0.1f}
		, m_screenFar{1000.0f}
	{
		UpdateProjection();
	}

	void Camera::UpdateScreenResolution(int width, int height)
	{
		m_screenAspectRatio = static_cast<float>(width) / static_cast<float>(height);
		UpdateProjection();
	}

	void Camera::UpdateFov(float fov)
	{
		m_fov = fov;
		UpdateProjection();
	}

	void Camera::UpdateScreenDepth(float near, float far)
	{
		m_screenNear = near;
		m_screenFar = far;
		UpdateProjection();
	}

	mth::float4x4 Camera::CameraMatrix() const
	{
		return m_projection * View();
	}

	mth::float4x4 Camera::View() const
	{
		return mth::RotationCamera4x4(position, rotation);
	}
}
