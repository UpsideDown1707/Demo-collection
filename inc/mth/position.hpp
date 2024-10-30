#pragma once

#include "linalg.hpp"

namespace democollection::mth
{
	template <typename T>
	class Position
	{
	public:
		Vector<T, 3> position;
		Vector<T, 3> rotation;
		Vector<T, 3> scale;

	public:
		Position() { Reset(); }

		void Reset()
		{
			position = Vector<T, 3>(T{0});
			rotation = Vector<T, 3>(T{0});
			scale = Vector<T, 3>(T{1});
		}

		void MoveForward(T d)
		{
			position(0) += std::sin(rotation(1)) * d;
			position(2) += std::cos(rotation(1)) * d;
		}
		void MoveBackward(T d)
		{
			position(0) -= std::sin(rotation(1)) * d;
			position(2) -= std::cos(rotation(1)) * d;
		}
		void MoveRight(T d)
		{
			position(0) += std::cos(rotation(1)) * d;
			position(2) -= std::sin(rotation(1)) * d;
		}
		void MoveLeft(T d)
		{
			position(0) -= std::cos(rotation(1)) * d;
			position(2) += std::sin(rotation(1)) * d;
		}
		void MoveUp(T d) { position(1) += d; }
		void MoveDown(T d) { position(1) -= d; }
		void Move(Vector<T, 3> delta) { position += delta; }
		void MoveInLookDirection(T d) { MoveInLookDirection(Vector<T, 3>(0, 0, d)); }
		void MoveInLookDirection(Vector<T, 3> delta) { position += RotationMatrix3x3() * delta; }

		void LookDown(T r) { rotation(0) += r; }
		void LookUp(T r) { rotation(0) -= r; }
		void TurnRight(T r) { rotation(1) += r; }
		void TurnLeft(T r) { rotation(1) -= r; }
		void RollRight(T r) { rotation(2) -= r; }
		void RollLeft(T r) { rotation(2) += r; }

		void ScaleX(T s) { scale(0) *= s; }
		void ScaleY(T s) { scale(1) *= s; }
		void ScaleZ(T s) { scale(2) *= s; }

		Matrix<T, 4, 4> PositionMatrix() { return Translation4x4(position); }
		Matrix<T, 4, 4> RotationMatrix() { return Rotation4x4(rotation); }
		Matrix<T, 4, 4> ScaleMatrix() { return Scaling4x4(scale); }
		Matrix<T, 4, 4> WorldMatrix() { return ScalingRotationTranslation4x4(scale, rotation, position); }
		Matrix<T, 4, 4> PositionMatrixInv() { return Translation4x4(-position); }
		Matrix<T, 4, 4> RotationMatrixInv() { return Rotation4x4(rotation).Transposed(); }
		Matrix<T, 4, 4> ScaleMatrixInv() { return Scaling4x4(Vector<T, 3>(T{1}) / scale); }
		Matrix<T, 4, 4> WorldMatrixInv() { return ScaleMatrixInv() * RotationMatrixInv() * PositionMatrixInv(); }
		Vector<T, 3> LookDirection() { return RotationMatrix3x3() * Vector<T, 3>(T{0}, T{0}, T{1}); }
		Matrix<T, 3, 3> RotationMatrix3x3() { return Rotation3x3(rotation); }
		Matrix<T, 3, 3> ScaleMatrix3x3() { return Scaling3x3(scale); }
		Matrix<T, 3, 3> RotationMatrixInv3x3() { return Rotation3x3(rotation).Transposed(); }
		Matrix<T, 3, 3> ScaleMatrixInv3x3() { return Scaling3x3(Vector<T, 3>(T{1}) / scale); }

		T DistanceSquare(Position<T>& other) { return (position - other.position).LengthSquare(); }
		T Distance(Position<T>& other) { return (position - other.position).Length(); }

		template <typename S>
		operator Position<S>()
		{
			Position<S> pos;
			pos.position = (Vector<S, 3>)position;
			pos.rotation = (Vector<S, 3>)rotation;
			pos.scale = (Vector<S, 3>)scale;
			return pos;
		}
	};

	using Positionf = Position<float>;
	using Positiond = Position<double>;
}
