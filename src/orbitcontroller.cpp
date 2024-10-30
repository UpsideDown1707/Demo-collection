#include "orbitcontroller.hpp"
#include "common.hpp"

namespace democollection
{
	void OrbitController::UpdateTargetPosition()
	{
		m_target.position = mth::Rotation3x3(m_target.rotation) * mth::float3(0.0f, 0.0f, -m_distance) - m_center;
	}

	OrbitController::OrbitController(mth::Positionf& target)
		: m_target{target}
		, m_center{}
		, m_distance{1.0f}
		, m_prevCursor{}
		, m_sensitivity{0.008f}
		, m_heldButtons{0}
	{
		UpdateTargetPosition();
	}

	void OrbitController::SetDistance(float distance)
	{
		m_distance = distance;
		UpdateTargetPosition();
	}

	void OrbitController::SetCenter(const mth::float3& center)
	{
		m_center = center;
		UpdateTargetPosition();
	}

	void OrbitController::MouseMove(float x, float y)
	{
		const mth::float2 cursor(x, y);
		if (m_heldButtons)
		{
			const mth::float2 delta = cursor - m_prevCursor;
			if (m_heldButtons & (1 << GLFW_MOUSE_BUTTON_LEFT))
			{
				m_target.LookUp(-delta(1) * m_sensitivity);
				m_target.TurnRight(delta(0) * m_sensitivity);
			}
			if (m_heldButtons & (1 << GLFW_MOUSE_BUTTON_RIGHT))
			{
				m_center += mth::Rotation3x3(m_target.rotation) *
						mth::float3(
								delta(0) * m_sensitivity * m_distance * 0.1f,
								-delta(1) * m_sensitivity * m_distance * 0.1f,
								0.0f);
			}
			UpdateTargetPosition();
		}
		m_prevCursor = cursor;
	}

	void OrbitController::MouseButton(int button, int action)
	{
		switch (action)
		{
		case GLFW_PRESS:
			m_heldButtons |= 1 << (button & (GLFW_MOUSE_BUTTON_LEFT | GLFW_MOUSE_BUTTON_RIGHT));
			break;
		case GLFW_RELEASE:
			m_heldButtons &= ~(1 << (button & (GLFW_MOUSE_BUTTON_LEFT | GLFW_MOUSE_BUTTON_RIGHT)));
			break;
		}
	}

	void OrbitController::Scroll(float delta)
	{
		m_distance *= delta < 0 ? 1.25f : 0.8f;
		UpdateTargetPosition();
	}
}
