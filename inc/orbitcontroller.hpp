#pragma once

#include "mth/position.hpp"

namespace democollection
{
	class OrbitController
	{
		mth::Positionf& m_target;
		mth::float3 m_center;
		float m_distance;
		mth::float2 m_prevCursor;
		float m_sensitivity;
		int m_heldButtons;

	private:
		void UpdateTargetPosition();

	public:
		OrbitController(mth::Positionf& target);
		void SetDistance(float distance);
		void SetCenter(const mth::float3& center);

		void MouseMove(float x, float y);
		void MouseButton(int button, int action);
		void Scroll(float delta);
	};
}
