#pragma once

#include <cmath>
#include <algorithm>
#include <initializer_list>
#include <ostream>

namespace democollection::mth
{
	template <typename T, size_t X, size_t Y> class Matrix;
	template <typename T, size_t S>
	class Vector
	{
		T m_vec[S];

	public:
		Vector() : m_vec{}{}
		Vector(const T& v)
		{
			for (T& e : m_vec)
				e = v;
		}
		Vector(const std::initializer_list<T>& v)
		{
			auto civ = std::begin(v);
			for (auto it = m_vec; it != m_vec + S; ++it)
			{
				*it = *civ;
				if (++civ == std::end(v))
				{
					while (++it != m_vec + S)
						*it = T{0};
					return;
				}
			}
		}
		template <typename... args>
		Vector(args... v) : Vector({v...}){}
		template <typename T2, size_t S2>
		Vector(const Vector<T2, S2>& v)
		{
			constexpr size_t len = std::min(S, S2);
			for (size_t i = 0; i < len; ++i)
				m_vec[i] = static_cast<T>(v(i));
			for (size_t i = len; len < S; ++i)
				m_vec[i] = T{0};
		}
		constexpr size_t Size()
		{
			return S;
		}
		Vector<T, S> operator+(const Vector<T, S>& v) const
		{
			Vector<T, S> r;
			for (size_t i = 0; i < S; ++i)
				r.m_vec[i] = m_vec[i] + v.m_vec[i];
			return r;
		}
		Vector<T, S> operator-(const Vector<T, S>& v) const
		{
			Vector<T, S> r;
			for (size_t i = 0; i < S; ++i)
				r.m_vec[i] = m_vec[i] - v.m_vec[i];
			return r;
		}
		Vector<T, S> operator*(const Vector<T, S>& v) const
		{
			Vector<T, S> r;
			for (size_t i = 0; i < S; ++i)
				r.m_vec[i] = m_vec[i] * v.m_vec[i];
			return r;
		}
		Vector<T, S> operator/(const Vector<T, S>& v) const
		{
			Vector<T, S> r;
			for (size_t i = 0; i < S; ++i)
				r.m_vec[i] = m_vec[i] / v.m_vec[i];
			return r;
		}
		Vector<T, S> operator+(const T& v) const
		{
			Vector<T, S> r;
			for (size_t i = 0; i < S; ++i)
				r.m_vec[i] = m_vec[i] + v;
			return r;
		}
		Vector<T, S> operator-(const T& v) const
		{
			Vector<T, S> r;
			for (size_t i = 0; i < S; ++i)
				r.m_vec[i] = m_vec[i] - v;
			return r;
		}
		Vector<T, S> operator*(const T& v) const
		{
			Vector<T, S> r;
			for (size_t i = 0; i < S; ++i)
				r.m_vec[i] = m_vec[i] * v;
			return r;
		}
		Vector<T, S> operator/(const T& v) const
		{
			Vector<T, S> r;
			for (size_t i = 0; i < S; ++i)
				r.m_vec[i] = m_vec[i] / v;
			return r;
		}
		Vector<T, S>& operator+=(const Vector<T, S>& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] += v.m_vec[i];
			return *this;
		}
		Vector<T, S>& operator-=(const Vector<T, S>& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] -= v.m_vec[i];
			return *this;
		}
		Vector<T, S>& operator*=(const Vector<T, S>& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] *= v.m_vec[i];
			return *this;
		}
		Vector<T, S>& operator/=(const Vector<T, S>& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] /= v.m_vec[i];
			return *this;
		}
		Vector<T, S>& operator=(const Vector<T, S>& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] = v.m_vec[i];
			return *this;
		}
		Vector<T, S>& operator+=(const T& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] += v;
			return *this;
		}
		Vector<T, S>& operator-=(const T& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] -= v;
			return *this;
		}
		Vector<T, S>& operator*=(const T& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] *= v;
			return *this;
		}
		Vector<T, S>& operator/=(const T& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] /= v;
			return *this;
		}
		Vector<T, S>& operator=(const T& v)
		{
			for (size_t i = 0; i < S; ++i)
				m_vec[i] = v;
			return *this;
		}
		template <size_t Y>
		Vector<T, Y> operator*(const Matrix<T, S, Y>& m) const
		{
			Vector<T, Y> q;
			for (size_t y = 0; y < Y; ++y)
				for (size_t i = 0; i < S; ++i)
					q(y) += m(i, y) * m_vec[i];
			return q;
		}
		const T& operator()(size_t i) const
		{
			return m_vec[i];
		}
		T& operator()(size_t i)
		{
			return m_vec[i];
		}
	};

	template <typename T, size_t S>
	T Dot(const Vector<T, S>& lhs, const Vector<T, S>& rhs)
	{
		T dot{};
		for (size_t i = 0; i < S; ++i)
			dot += lhs(i) * rhs(i);
		return dot;
	}
	template <typename T, size_t S>
	T LengthSquare(const Vector<T, S>& v)
	{
		T s{};
		for (size_t i = 0; i < S; ++i)
			s += v(i) * v(i);
		return s;
	}
	template <typename T, size_t S>
	T Length(const Vector<T, S>& v)
	{
		return std::sqrt(LengthSquare(v));
	}
	template <typename T, size_t S>
	Vector<T, S> Normalized(const Vector<T, S>& v)
	{
		return v / Length(v);
	}
	template <typename T, size_t S>
	Vector<T, S> operator*(const T& v, const Vector<T, S>& n)
	{
		return v * n;
	}
	template <typename T, size_t S>
	std::ostream& operator<<(std::ostream& os, const Vector<T, S>& v)
	{
		os << '(' << v(0);
		for (size_t i = 1; i < S; ++i)
			os << ' ' << v(i);
		os << ')' << std::endl;
		return os;
	}

	using float2 = Vector<float, 2>;
	using float3 = Vector<float, 3>;
	using float4 = Vector<float, 4>;
	using double2 = Vector<double, 2>;
	using double3 = Vector<double, 3>;
	using double4 = Vector<double, 4>;
	using int2 = Vector<int, 2>;
	using int3 = Vector<int, 3>;
	using int4 = Vector<int, 4>;
	using uint2 = Vector<unsigned int, 2>;
	using uint3 = Vector<unsigned int, 3>;
	using uint4 = Vector<unsigned int, 4>;

	template <typename T, size_t X, size_t Y>
	class Matrix
	{
		T m_mat[X * Y];
	private:
		inline const T& At(size_t x, size_t y) const { return m_mat[y * X + x]; }
		inline T& At(size_t x, size_t y) { return m_mat[y * X + x]; }

	public:
		Matrix() : m_mat{}{}
		Matrix(const T& v)
		{
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					At(x, y) = v;
		}
		Matrix(const std::initializer_list<T>& v)
		{
			auto civ = std::begin(v);
			for (auto it = m_mat; it != m_mat + X * Y; ++it)
			{
				*it = *civ;
				if (++civ == std::end(v))
				{
					while (++it != m_mat + X * Y)
						*it = T{0};
					return;
				}
			}
		}
		template <typename... args>
		Matrix(args... v) : Matrix({v...}){}
		template <typename T2, size_t X2, size_t Y2>
		Matrix(const Matrix<T2, X2, Y2>& m) : m_mat{}
		{
			for (size_t i = std::min(X2, Y2); i < std::min(X, Y); ++i)
				At(i, i) = T{1};
			for (size_t y = 0; y < std::min(Y, Y2); ++y)
				for (size_t x = 0; x < std::min(X, X2); ++x)
					At(x, y) = static_cast<T>(m(x, y));
		}
		constexpr size_t RowSize() const
		{
			return X;
		}
		constexpr size_t ColumnSize() const
		{
			return Y;
		}
		Vector<T, Y> ColToVector(size_t x) const
		{
			Vector<T, Y> v;
			for (size_t i = 0; i < Y; ++i)
				v(i) = At(x, i);
			return v;
		}
		Vector<T, X> RowToVector(size_t y) const
		{
			Vector<T, X> v;
			for (size_t i = 0; i < X; ++i)
				v(i) = At(i, y);
			return v;
		}
		inline const T& operator()(size_t x, size_t y) const { return At(x, y); }
		inline T& operator()(size_t x, size_t y) { return At(x, y); }
		Vector<T, Y> operator*(const Vector<T, X>& v) const
		{
			Vector<T, Y> q;
			for (size_t y = 0; y < Y; ++y)
				for (size_t i = 0; i < X; ++i)
					q(y) += At(i, y) * v(i);
			return q;
		}
		Matrix<T, Y, Y> operator*(const Matrix<T, Y, X>& m) const
		{
			Matrix<T, Y, Y> q;
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < Y; ++x)
					for (size_t i = 0; i < X; ++i)
						q(x, y) += At(i, y) * m(x, i);
			return q;
		}
		Matrix<T, X, Y> operator+(const Matrix<T, X, Y>& m) const
		{
			Matrix<T, X, Y> q;
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					q(x, y) = At(x, y) + m(x, y);
			return q;
		}
		Matrix<T, X, Y>& operator+=(const Matrix<T, X, Y>& m)
		{
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					At(x, y) += m(x, y);
			return *this;
		}
		Matrix<T, X, Y> operator-(const Matrix<T, Y, Y>& m) const
		{
			Matrix<T, Y, Y> q;
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					q(x, y) = At(x, y) - m(x, y);
			return q;
		}
		Matrix<T, X, Y>& operator-=(const Matrix<T, X, Y>& m)
		{
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					At(x, y) -= m(x, y);
			return *this;
		}
		Matrix<T, X, Y>& operator=(const Matrix<T, X, Y>& m)
		{
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					At(x, y) = m(x, y);
			return *this;
		}
		Matrix<T, X, Y> operator+(const T& v) const
		{
			Matrix<T, Y, Y> q;
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					q(x, y) = At(x, y) + v;
			return q;
		}
		Matrix<T, X, Y>& operator+=(const T& v)
		{
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					At(x, y) += v;
			return *this;
		}
		Matrix<T, X, Y> operator-(const T& v) const
		{
			Matrix<T, X, Y> q;
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					q(x, y) = At(x, y) - v;
			return q;
		}
		Matrix<T, X, Y>& operator-=(const T& v)
		{
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					At(x, y) -= v;
			return *this;
		}
		Matrix<T, X, Y> operator*(const T& v) const
		{
			Matrix<T, X, Y> q;
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					q(x, y) = At(x, y) * v;
			return q;
		}
		Matrix<T, X, Y>& operator*=(const T& v)
		{
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					At(x, y) *= v;
			return *this;
		}
		Matrix<T, X, Y> operator/(const T& v) const
		{
			Matrix<T, X, Y> q;
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					q(x, y) = At(x, y) / v;
			return q;
		}
		Matrix<T, X, Y>& operator/=(const T& v)
		{
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					At(x, y) /= v;
			return *this;
		}
		Matrix<T, X, Y>& operator=(const T& v)
		{
			for (size_t y = 0; y < Y; ++y)
				for (size_t x = 0; x < X; ++x)
					At(x, y) = v;
			return *this;
		}
	};

	template <typename T, size_t S>
	Matrix<T, S, S> Identity()
	{
		Matrix<T, S, S> m;
		for (size_t i = 0; i < S; ++i)
			m(i, i) = T{1};
		return m;
	}
	template <typename T, size_t X, size_t Y>
	Matrix<T, X - 1, Y - 1> SubMatrix(const Matrix<T, X, Y> matrix, size_t excludedX, size_t excludedY)
	{
		Matrix<T, X - 1, Y - 1> m;
		size_t curry = 0;

		for (size_t y = 0; y < Y; ++y)
		{
			if (y != excludedY)
			{
				size_t currx = 0;
				for (size_t x = 0; x < X; ++x)
				{
					if (x != excludedX)
					{
						m(currx, curry) = matrix(x, y);
						++currx;
					}
				}
				++curry;
			}
		}
		return m;
	}
	template <typename T, size_t S>
	T Determinant(const Matrix<T, S, S>& matrix)
	{
		T d{};
		for (size_t x = 0; x < S; ++x)
		{
			T tmp = matrix(x, 0) * Determinant(SubMatrix(matrix, x, 0));
			if (x & 1)
				d -= tmp;
			else
				d += tmp;
		}
		return d;
	}
	template <typename T>
	T Determinant(const Matrix<T, 1, 1>& matrix)
	{
		return matrix(0, 0);
	}
	template <typename T, size_t S>
	Matrix<T, S, S> Inverse(const Matrix<T, S, S>& matrix)
	{
		Matrix<T, S, S> m;
		T oneoverdet = T{1} / Determinant(matrix);
		for (size_t y = 0; y < S; ++y)
		{
			for (size_t x = 0; x < S; ++x)
			{
				T tmp = Determinant(SubMatrix(matrix, y, x)) * oneoverdet;
				m(x, y) = ((x ^ y) & 1) ? -tmp : tmp;
			}
		}
		return m;
	}
	template <typename T>
	Matrix<T, 1, 1> Inverse(const Matrix<T, 1, 1>& matrix)
	{
		Matrix<T, 1, 1> m;
		m(0, 0) = T{1} / matrix(0, 0);
		return m;
	}
	template <typename T, size_t R, size_t C>
	Matrix<T, C, R> Transposed(const Matrix<T, R, C>& matrix)
	{
		Matrix<T, C, R> m;
		for (size_t c = 0; c < C; ++c)
			for (size_t r = 0; r < R; ++r)
				m(c, r) = matrix(r, c);
		return m;
	}

	template <typename T>
	Matrix<T, 3, 3> Scaling3x3(const T& x, const T& y, const T& z)
	{
		return Matrix<T, 3, 3>(
			x, T{0}, T{0},
			T{0}, y, T{0},
			T{0}, T{0}, z);
	}
	template <typename T>
	Matrix<T, 3, 3> Scaling3x3(const Vector<T, 3>& s)
	{
		return Scaling(s(0), s(1), s(2));
	}
	template <typename T>
	Matrix<T, 3, 3> RotationX3x3(const T& a)
	{
		T ca = std::cos(a), sa = std::sin(a);
		return Matrix<T, 3, 3>(
			T{1}, T{0}, T{0},
			T{0}, ca, -sa,
			T{0}, sa, ca);
	}
	template <typename T>
	Matrix<T, 3, 3> RotationY3x3(const T& a)
	{
		T ca = std::cos(a), sa = std::sin(a);
		return Matrix<T, 3, 3>(
			ca, T{0}, sa,
			T{0}, T{1}, T{0},
			-sa, T{0}, ca);
	}
	template <typename T>
	Matrix<T, 3, 3> RotationZ3x3(const T& a)
	{
		T ca = std::cos(a), sa = std::sin(a);
		return Matrix<T, 3, 3>(
			ca, -sa, T{0},
			sa, ca, T{0},
			T{0}, T{0}, T{1});
	}
	template <typename T>
	Matrix<T, 3, 3> Rotation3x3(const T& pitch, const T& yaw, const T& roll)
	{
		T cp = std::cos(pitch), sp = std::sin(pitch), cy = std::cos(yaw), sy = std::sin(yaw), cr = std::cos(roll), sr = std::sin(roll);
		return Matrix<T, 3, 3>(
			sy * sp * sr + cy * cr, sy * sp * cr - cy * sr, sy * cp,
			cp * sr, cp * cr, -sp,
			cy * sp * sr - sy * cr, cy * sp * cr + sy * sr, cy * cp);
	}
	template <typename T>
	Matrix<T, 3, 3> Rotation3x3(const Vector<T, 3>& r)
	{
		return Rotation3x3(r(0), r(1), r(2));
	}
	template <typename T>
	Matrix<T, 3, 3> RotationAxis3x3(const Vector<T, 3>& axis, const T& a)
	{
		return RotationNormal(Normalized(axis), a);
	}
	template <typename T>
	Matrix<T, 3, 3> RotationNormal3x3(const Vector<T, 3>& n, const T& a)
	{
		T ca = std::cos(a), sa = std::sin(a);
		return Matrix<T, 3, 3>(
			ca + n(0) * n(0) * (T{1} - ca), n(0) * n(1) * (T{1} - ca) - n(2) * sa, n(0) * n(2) * (T{1} - ca) + n(1) * sa,
			n(1) * n(0) * (T{1} - ca) + n(2) * sa, ca + n(1) * n(1) * (T{1} - ca), n(1) * n(2) * (T{1} - ca) - n(0) * sa,
			n(2) * n(0) * (T{1} - ca) - n(1) * sa, n(2) * n(1) * (T{1} - ca) + n(0) * sa, ca + n(2) * n(2) * (T{1} - ca));
	}
	template <typename T>
	Matrix<T, 3, 3> RotationCamera3x3(const T& pitch, const T& yaw, const T& roll)
	{
		T sx = std::sin(-pitch), cx = std::cos(-pitch), sy = std::sin(-yaw), cy = std::cos(-yaw), sz = std::sin(-roll), cz = std::cos(-roll);
		return Matrix<T, 3, 3>(
			cy * cz - sx * sy * sz, -cx * sz, sy * cz + sx * cy * sz,
			cy * sz + sx * sy * cz, cx * cz, sy * sz - sx * cy * cz,
			-cx * sy, sx, cx * cy);
	}
	template <typename T>
	Matrix<T, 3, 3> RotationCamera3x3(const Vector<T, 3>& r)
	{
		return RotationCamera3x3(r(0), r(1), r(2));
	}
	template <typename T>
	Matrix<T, 3, 3> RotateUnitVector3x3(const Vector<T, 3>& from, const Vector<T, 3>& to)
	{
		Vector<T, 3> v = Cross(from, to);
		T t = T{1} / (T{1} + Dot(from, to));
		return Matrix<T, 3, 3>(
			T{1} - v(1) * v(1) * t - v(2) * v(2) * t, v(0) * v(1) * t - v(2), v(0) * v(2) * t + v(1),
			v(0) * v(1) * t + v(2), T{1} - v(0) * v(0) * t - v(2) * v(2) * t, v(1) * v(2) * t - v(0),
			v(0) * v(2) * t - v(1), v(1) * v(2) * t + v(0), T{1} - v(0) * v(0) * t - v(1) * v(1) * t
			);
	}
	template <typename T>
	Vector<T, 3> ToRotationAngles3x3(const Matrix<T, 3, 3>& matrix)
	{
		Vector<T, 3> a;
		a(1) = std::atan2(matrix(2, 0), matrix(2, 2));
		a(2) = std::atan2(matrix(0, 1), matrix(1, 1));
		a(0) = std::atan2(-matrix(2, 1), (std::fmod(std::abs(a(1)), M_PI_2) < M_PI_4) ? (matrix(2, 2) / std::cos(a(1))) : (matrix(2, 0) / std::sin(a(1))));
		return a;
	}
	template <typename T>
	Vector<T, 3> ToCameraRotation3x3(const Matrix<T, 3, 3>& matrix)
	{
		Vector<T, 3> a;
		a(1) = -std::atan2(-matrix(1, 0), matrix(1, 1));
		a(2) = -std::atan2(-matrix(0, 2), matrix(1, 2));
		a(0) = -std::asin(matrix(1, 2));
		return a;
	}

	template <typename T>
	Matrix<T, 4, 4> Scaling4x4(const T& x, const T& y, const T& z)
	{
		return Matrix<T, 4, 4>(Scaling3x3(x, y, z));
	}
	template <typename T>
	Matrix<T, 4, 4> Scaling4x4(const Vector<T, 3>& s)
	{
		return Scaling4x4(s(0), s(1), s(2));
	}
	template <typename T>
	Matrix<T, 4, 4> Translation4x4(const T& x, const T& y, const T& z)
	{
		return Matrix<T, 4, 4>(
			T{1}, T{0}, T{0}, x,
			T{0}, T{1}, T{0}, y,
			T{0}, T{0}, T{1}, z,
			T{0}, T{0}, T{0}, T{1});
	}
	template <typename T>
	Matrix<T, 4, 4> Translation4x4(const Vector<T, 3>& t)
	{
		return Translation(t(0), t(1), t(2));
	}
	template <typename T>
	Matrix<T, 4, 4> RotationX4x4(const T& a)
	{
		return Matrix<T, 4, 4>(RotationX3x3(a));
	}
	template <typename T>
	Matrix<T, 4, 4> RotationY4x4(const T& a)
	{
		return Matrix<T, 4, 4>(RotationY3x3(a));
	}
	template <typename T>
	Matrix<T, 4, 4> RotationZ4x4(const T& a)
	{
		return Matrix<T, 4, 4>(RotationZ3x3(a));
	}
	template <typename T>
	Matrix<T, 4, 4> Rotation4x4(const T& pitch, const T& yaw, const T& roll)
	{
		return Matrix<T, 4, 4>(Rotation3x3(pitch, yaw, roll));
	}
	template <typename T>
	Matrix<T, 4, 4> Rotation4x4(const Vector<T, 3>& r)
	{
		return Rotation4x4(r(0), r(1), r(2));
	}
	template <typename T>
	Matrix<T, 4, 4> RotationAxis4x4(const Vector<T, 3>& axis, const T& a)
	{
		return RotationNormal4x4(axis.Normalized(), a);
	}
	template <typename T>
	Matrix<T, 4, 4> RotationNormal4x4(const Vector<T, 3>& n, const T& a)
	{
		return Matrix<T, 4, 4>(RotationNormal3x3(n, a));
	}
	template <typename T>
	Matrix<T, 4, 4> RotationCamera4x4(const T& px, const T& py, const T& pz, const T& rx, const T& ry, const T& rz)
	{
		const Matrix<T, 3, 3> m = RotationCamera3x3(rx, ry, rz);
		const Vector<T, 3> v = m * Vector<T, 3>(px, py, pz);
		return Matrix<T, 4, 4>(
			m(0, 0), m(1, 0), m(2, 0), -v(0),
			m(0, 1), m(1, 1), m(2, 1), -v(1),
			m(0, 2), m(1, 2), m(2, 2), -v(2),
			T{0}, T{0}, T{0}, T{1});
	}
	template <typename T>
	Matrix<T, 4, 4> RotationCamera4x4(const Vector<T, 3>& p, const Vector<T, 3>& r)
	{
		return RotationCamera4x4(p(0), p(1), p(2), r(0), r(1), r(2));
	}
	template <typename T>
	Matrix<T, 4, 4> ScalingRotationTranslation4x4(
		const T& sx, const T& sy, const T& sz,
		const T& pitch, const T& yaw, const T& roll,
		const T& tx, const T& ty, const T& tz)
	{
		const Matrix<T, 3, 3> m = Rotation3x3(pitch, yaw, roll);
		return Matrix<T, 4, 4>(
			m(0, 0) * sx, m(1, 0) * sy, m(2, 0) * sz, tx,
			m(0, 1) * sx, m(1, 1) * sy, m(2, 1) * sz, ty,
			m(0, 2) * sx, m(1, 2) * sy, m(2, 2) * sz, tz,
			T{0}, T{0}, T{0}, T{1});
	}
	template <typename T>
	Matrix<T, 4, 4> ScalingRotationTranslation4x4(const Vector<T, 3>& s, const Vector<T, 3>& r, const Vector<T, 3> &t)
	{
		return ScalingRotationTranslation4x4(s(0), s(1), s(2), r(0), r(1), r(2), t(0), t(1), t(2));
	}
	template <typename T>
	Matrix<T, 4, 4> RotationTranslation4x4(
		const T& pitch, const T& yaw, const T& roll,
		const T& tx, const T& ty, const T& tz)
	{
		const Matrix<T, 3, 3> m = Matrix<T, 3, 3>::Rotation(pitch, yaw, roll);
		return Matrix<T, 4, 4>(
			m(0, 0), m(1, 0), m(2, 0), tx,
			m(0, 1), m(1, 1), m(2, 1), ty,
			m(0, 2), m(1, 2), m(2, 2), tz,
			T{0}, T{0}, T{0}, T{1});
	}
	template <typename T>
	Matrix<T, 4, 4> RotationTranslation4x4(const Vector<T, 3>& r, const Vector<T, 3>& t)
	{
		return RotationTranslation4x4(r(0), r(1), r(2), t(0), t(1), t(2));
	}
	template <typename T>
	Matrix<T, 4, 4> ScalingTranslation4x4(
		const T& sx, const T& sy, const T& sz,
		const T& tx, const T& ty, const T& tz)
	{
		return Matrix<T, 4, 4>(
			sx, T{0}, T{0}, tx,
			T{0}, sy, T{0}, ty,
			T{0}, T{0}, sz, tz,
			T{0}, T{0}, T{0}, T{1});
	}
	template <typename T>
	Matrix<T, 4, 4> ScalingTranslation4x4(const Vector<T, 3>& s, const Vector<T, 3>& t)
	{
		return ScalingTranslation4x4(s(0), s(1), s(2), t(0), t(1), t(2));
	}
	template <typename T>
	Matrix<T, 4, 4> ScalingRotation4x4(
		const T& sx, const T &sy, const T& sz,
		const T& pitch, const T& yaw, const T& roll)
	{
		const Matrix<T, 3, 3> m = Rotation3x3(pitch, yaw, roll);
		return Matrix<T, 4, 4>(
			m(0, 0) * sx, m(1, 0) * sy, m(2, 0) * sz, 0,
			m(0, 1) * sx, m(1, 1) * sy, m(2, 1) * sz, 0,
			m(0, 2) * sx, m(1, 2) * sy, m(2, 2) * sz, 0,
			T{0}, T{0}, T{0}, T{1});
	}
	template <typename T>
	Matrix<T, 4, 4> ScalingRotation4x4(const Vector<T, 3>& s, const Vector<T, 3>& r)
	{
		return ScalingRotation4x4(s(0), s(1), s(2), r(0), r(1), r(2));
	}
	template <typename T>
	Matrix<T, 4, 4> PerspectiveFOV(const T& fov, const T& screenAspect, const T& screenNear, const T& screenDepth)
	{
		T yScale = 1 / std::tan(fov / 2);
		T xScale = yScale / screenAspect;
		return Matrix<T, 4, 4>(
			xScale, T{0}, T{0}, T{0},
			T{0}, -yScale, T{0}, T{0},
			T{0}, T{0}, screenDepth / (screenDepth - screenNear), -screenDepth * screenNear / (screenDepth - screenNear),
			T{0}, T{0}, T{1}, T{0});
	}
	template <typename T>
	Matrix<T, 4, 4> Orthographic(const T& viewWidth, const T& viewHeight, const T& screenNear, const T& screenDepth)
	{
		return Matrix<T, 4, 4>(
			T{2} / viewWidth, T{0}, T{0}, T{0},
			T{0}, -T{2} / viewHeight, T{0}, T{0},
			T{0}, T{0}, T{1} / (screenDepth - screenNear), -screenNear / (screenDepth - screenNear),
			T{0}, T{0}, T{0}, T{1});
	}
	template <typename T>
	Matrix<T, 4, 4> LookTo(const Vector<T, 3>& eye, const Vector<T, 3>& direction, const Vector<T, 3>& up)
	{
		Vector<T, 3> zaxis = direction.Normalized();
		Vector<T, 3> xaxis = up.Cross(zaxis).Normalized();
		Vector<T, 3> yaxis = zaxis.Cross(xaxis);
		return Matrix<T, 4, 4>(
			xaxis(0), xaxis(1), xaxis(2), -xaxis.Dot(eye),
			yaxis(0), yaxis(1), yaxis(2), -yaxis.Dot(eye),
			zaxis(0), zaxis(1), zaxis(2), -zaxis.Dot(eye),
			T{0}, T{0}, T{0}, T{1});
	}
	template <typename T>
	Matrix<T, 4, 4> LookAt(const Vector<T, 3>& eye, const Vector<T, 3>& focus, const Vector<T, 3>& up)
	{
		return LookTo(eye, focus - eye, up);
	}

	template <typename T, size_t X, size_t Y>
	Matrix<T, X, Y> operator*(const T& n, const Matrix<T, X, Y>& m)
	{
		return m * n;
	}
	template <typename T, size_t X, size_t Y>
	std::ostream& operator<<(std::ostream& os, const Matrix<T, X, Y>& m)
	{
		for (size_t y = 0; y < Y; ++y)
		{
			os << '|' << m(0, y);
			for (size_t x = 1; x < X; ++x)
				os << ' ' << m(x, y);
			os << '|' << std::endl;
		}
		return os;
	}

	template <typename T>
	Vector<T, 3> Transform(const Matrix<T, 4, 4>& mat, const Vector<T, 3>& v)
	{
		return Vector<T, 3>(mat * Vector<T, 4>(v(0), v(1), v(2),T{1}));
	}

	using float2x2 = Matrix<float, 2, 2>;
	using float3x3 = Matrix<float, 3, 3>;
	using float4x4 = Matrix<float, 4, 4>;
	using double2x2 = Matrix<double, 2, 2>;
	using double3x3 = Matrix<double, 3, 3>;
	using double4x4 = Matrix<double, 4, 4>;
	using int2x2 = Matrix<int, 2, 2>;
	using int3x3 = Matrix<int, 3, 3>;
	using int4x4 = Matrix<int, 4, 4>;
	using uint2x2 = Matrix<unsigned int, 2, 2>;
	using uint3x3 = Matrix<unsigned int, 3, 3>;
	using uint4x4 = Matrix<unsigned int, 4, 4>;
}
