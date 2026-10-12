export module Shared:Math;
import std;
import Win32;
import DirectXMath;

// General math utility functions
export namespace Math
{
	auto IsFinite(const DirectX::XMFLOAT3& point) noexcept -> bool
	{
		return std::isfinite(point.x) and std::isfinite(point.y) and std::isfinite(point.z);
	}

	auto NearEqual(float a, float b, float tolerance = 0.00001f) noexcept -> bool
	{
		return std::fabs(a - b) <= tolerance;
	}
}

/*
* The dot product of two vectors produces a scalar quantity. It is the sum of the products
* of the corresponding components of the vectors: a · b = a.x * b.x + a.y * b.y + a.z * b.z.
* It is also given by the formula a · b = |a| * |b| * cos(theta), where theta is the angle
* between the two vectors.
*
* That quantity has the following properties:
* * The dot product is commutative: a · b = b · a
* * The dot product is distributive over vector addition: a · (b + c) = a · b + a · c
* * Scalar multiplication is distributive over the dot product: (da) · b = d(a · b) = a · (db)
* * The dot product of a vector with itself is equal to the square of its magnitude: a · a = |a|^2
* * The dot product of two orthogonal vectors is zero: a · b = 0 if a and b are orthogonal (perpendicular, 90 degrees)
* * The dot product is:
		-> greater than zero if the angle between the two vectors is less than 90 degrees (acute);
		-> less than zero if the angle is greater than 90 degrees but less than 180 degrees (obtuse);
		-> equal to zero if the angle is exactly 90 degrees (right angle).
* * The dot product can be used to project one vector onto another: proj_b(a) = (a · b / |b|^2) * b
* * The dot product can be used to find the angle between two vectors: a · b = |a| * |b| * cos(theta),
	where theta is the angle between a and b.
*/
export namespace Math
{
	auto Dot3(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) noexcept -> float
	{
		constexpr auto UseDirectXMath = true;
		if constexpr (UseDirectXMath)
		{
			auto va = DirectX::XMLoadFloat3(&a);
			auto vb = DirectX::XMLoadFloat3(&b);
			auto dot = DirectX::XMVector3Dot(va, vb);
			return DirectX::XMVectorGetX(dot);
		}
		else
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
	}

	auto __vectorcall Dot3(
		const DirectX::XMVECTOR lhs,
		const DirectX::XMVECTOR rhs
	) noexcept -> float
	{
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(lhs, rhs));
	}
}

// The formula for the cross product of two vectors a and b is:
// a × b = (a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x)
export namespace CrossProduct
{
	auto Cross(
		const DirectX::XMFLOAT3& a, 
		const DirectX::XMFLOAT3& b
	) noexcept -> DirectX::XMFLOAT3
	{
		auto va = DirectX::XMLoadFloat3(&a);
		auto vb = DirectX::XMLoadFloat3(&b);
		auto cross = DirectX::XMVector3Cross(va, vb);
		auto result = DirectX::XMFLOAT3{};
		DirectX::XMStoreFloat3(&result, cross);
		return result;
	}

	// Remember: https://learn.microsoft.com/en-us/windows/win32/dxmath/pg-xnamath-internals#calling-conventions
	// For x64, the first 6 vector parameters can be passed in registers. However, for non-static 
	// member functions, the `this` pointer is passed in rcx, which means that the first 5 vector 
	// parameters can be passed in registers. 
	auto __vectorcall Cross(
		const DirectX::XMVECTOR lhs, 
		const DirectX::XMVECTOR rhs
	) noexcept -> DirectX::XMVECTOR
	{
		return DirectX::XMVector3Cross(lhs, rhs);
	}
}

export namespace Math
{
	// Quaternions are of the form q = w + xi + yj + zk, where w is the scalar part and (x, y, z) is the vector part.
	using Quaternion = DirectX::XMFLOAT4;

	auto QuaternionFromAxisAngle(const DirectX::XMFLOAT3& axis, float angle) noexcept -> Quaternion
	{
		auto halfAngle = angle * 0.5f;
		auto sinHalfAngle = std::sin(halfAngle);
		auto cosHalfAngle = std::cos(halfAngle);
		return Quaternion{
			axis.x * sinHalfAngle,
			axis.y * sinHalfAngle,
			axis.z * sinHalfAngle,
			cosHalfAngle
		};
	}

	auto EulerToQuaternion(float pitch, float yaw, float roll) noexcept -> Quaternion
	{
		auto halfPitch = pitch * 0.5f;
		auto halfYaw = yaw * 0.5f;
		auto halfRoll = roll * 0.5f;
		auto sinPitch = std::sin(halfPitch);
		auto cosPitch = std::cos(halfPitch);
		auto sinYaw = std::sin(halfYaw);
		auto cosYaw = std::cos(halfYaw);
		auto sinRoll = std::sin(halfRoll);
		auto cosRoll = std::cos(halfRoll);
		return Quaternion{
			cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
			sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll,
			cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll,
			cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll
		};
	}
}

export namespace Math
{
	struct Plane
	{
		DirectX::XMFLOAT3 Normal{ 0.0f, 0.0f, 0.0f };
		float D = 0;

		Plane() = default;
		Plane(const DirectX::XMFLOAT3& normal, float d) noexcept
			: Normal{ normal }, D{ d }
		{}

		Plane(DirectX::XMVECTOR normal) noexcept
		{
			DirectX::XMStoreFloat3(&Normal, normal);
			D = DirectX::XMVectorGetW(normal);
		}

		auto IsNormalized() const noexcept -> bool
		{
			auto normalVector = DirectX::XMLoadFloat3(&Normal);
			auto lengthSquared = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(normalVector));
			if (lengthSquared == 0.f)
				return false;
			return NearEqual(lengthSquared, 1.f);
		}

		auto ToNormalizedPlane() const -> Plane
		{
			auto normalVector = DirectX::XMLoadFloat3(&Normal);
			auto length = DirectX::XMVector3Length(normalVector);
			if (DirectX::XMVectorGetX(length) == 0.f)
				throw std::runtime_error{ "Cannot normalize a plane with a zero-length normal." };

			if (IsNormalized())
				return *this;

			auto normalizedNormal = DirectX::XMVectorDivide(normalVector, length);
			auto d = D / DirectX::XMVectorGetX(length);
			auto dVector = DirectX::XMVectorSetW(normalizedNormal, d);

			auto normalizedNormalFloat3 = DirectX::XMFLOAT3{};
			DirectX::XMStoreFloat3(&normalizedNormalFloat3, normalizedNormal);
			return Plane{ normalizedNormalFloat3, d };
		}

		constexpr auto operator==(const Plane& other) const noexcept -> bool
		{
			auto p1 = DirectX::XMVectorSetW(DirectX::XMLoadFloat3(&Normal), D);
			auto p2 = DirectX::XMVectorSetW(DirectX::XMLoadFloat3(&other.Normal), other.D);
			return DirectX::XMPlaneEqual(p1, p2);
		}

		auto IsNearEqual(const Plane& other, float tolerance) const noexcept -> bool
		{
			auto p1 = DirectX::XMVectorSetW(DirectX::XMLoadFloat3(&Normal), D);
			auto p2 = DirectX::XMVectorSetW(DirectX::XMLoadFloat3(&other.Normal), other.D);
			return DirectX::XMPlaneNearEqual(p1, p2, DirectX::XMVectorReplicate(tolerance));
		}
	};

	struct PlaneHash
	{
		static auto operator()(const Plane& plane) noexcept -> std::size_t
		{
			auto seed = 0uz;
			for (float value : { plane.Normal.x, plane.Normal.y, plane.Normal.z, plane.D })
			{
				seed ^= std::hash<float>{}(value)+0x9e3779b9u
					+ (seed << 6) + (seed >> 2);
			}
			return seed;
		}
	};

	auto DistancePointToUnitPlane(
		const DirectX::XMFLOAT3& point, 
		const DirectX::XMFLOAT4& plane
	) -> float
	{
		auto xmPoint = DirectX::XMLoadFloat3(&point);
		auto xmPlane = DirectX::XMLoadFloat4(&plane);
		auto result = DirectX::XMPlaneDotCoord(xmPlane, xmPoint);
		return DirectX::XMVectorGetX(result);
	}

	auto __vectorcall DistancePointToUnitPlane(
		DirectX::XMVECTOR point,
		DirectX::XMVECTOR plane
	) -> float
	{
		auto result = DirectX::XMPlaneDotCoord(point, plane);
		return DirectX::XMVectorGetX(result);
	}

	// Distance is between point P(x0, y0, z0) and the plane P = ax + by + cz + d = 0
	// D = (|ax0 + by0 + cz0 + d|)/sqrt(a^2+b^2+c^2)
	auto DistancePointToNonUnitPlane(
		const DirectX::XMFLOAT3& point, 
		const DirectX::XMFLOAT4& plane
	) -> float
	{
		auto xmPoint = DirectX::XMLoadFloat3(&point);
		auto normalizedPlane = DirectX::XMPlaneNormalize(DirectX::XMLoadFloat4(&plane));
		return DistancePointToUnitPlane(xmPoint, normalizedPlane);
	}

	auto PointToNonUnitPlaneDistanceAbs(
		const DirectX::XMFLOAT3& point, 
		const DirectX::XMFLOAT4& plane
	) -> float
	{
		auto xmPoint = DirectX::XMLoadFloat3(&point);
		auto normalizedPlane = DirectX::XMPlaneNormalize(DirectX::XMLoadFloat4(&plane));
		return DistancePointToUnitPlane(xmPoint, normalizedPlane);
	}
}

//
// Spheres
export namespace Math
{
	struct Sphere
	{
		DirectX::XMFLOAT3 Center{ 0, 0, 0 };
		float Radius{ 0 };
		DirectX::XMFLOAT4 Orientation{ 0, 0, 0, 1 };
	};

	struct SphereCompact
	{
		DirectX::XMFLOAT4 Data{ 0, 0, 0, 0 };
		DirectX::XMFLOAT4 Orientation{ 0, 0, 0, 1 };

		auto Rotate(float x, float y, float z) const noexcept -> SphereCompact
		{
			auto rotation = Quaternion{ x, y, z, 1 };

			return SphereCompact{
				Data,
				DirectX::XMFLOAT4{
					Orientation.x + rotation.x,
					Orientation.y + rotation.y,
					Orientation.z + rotation.z,
					Orientation.w + rotation.w
				}
			};
		}

		auto GetCenter() const noexcept -> DirectX::XMFLOAT3
		{
			return DirectX::XMFLOAT3{ Data.x, Data.y, Data.z };
		}

		auto GetRadius() const noexcept -> float
		{
			return Data.w;
		}

		auto Translate(const DirectX::XMFLOAT3& offset) const noexcept -> SphereCompact
		{
			return SphereCompact{ DirectX::XMFLOAT4{ Data.x + offset.x, Data.y + offset.y, Data.z + offset.z, Data.w } };
		}
	};
}

//
// Triangles
export namespace Math
{
	struct Triangle
	{
		DirectX::XMFLOAT3 V0 = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 V1 = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 V2 = { 0.0f, 0.0f, 0.0f };

		auto GetPlane() const noexcept -> Plane
		{
			auto vector = DirectX::XMVECTOR{
				DirectX::XMPlaneFromPoints(
					DirectX::XMLoadFloat3(&V0),
					DirectX::XMLoadFloat3(&V1),
					DirectX::XMLoadFloat3(&V2)
				) };
			auto normal = DirectX::XMFLOAT3{};
			auto d = 0.0f;
			DirectX::XMStoreFloat3(&normal, vector);
			d = DirectX::XMVectorGetW(vector);
			return Plane{ normal, d };
		}

		auto GetPlaneAsFloat4() const noexcept -> DirectX::XMFLOAT4
		{
			auto vector = DirectX::XMVECTOR{
				DirectX::XMPlaneFromPoints(
					DirectX::XMLoadFloat3(&V0),
					DirectX::XMLoadFloat3(&V1),
					DirectX::XMLoadFloat3(&V2)
				) };
			auto result = DirectX::XMFLOAT4{};
			DirectX::XMStoreFloat4(&result, vector);
			return result;
		}

		// XMVECTOR is fine here. XMVECTOR in class members
		// XMVECTOR is in the form of (A, B, C, D) where Ax + By + Cz + D = 0 is the plane equation.
		auto GetPlaneAsVector() const noexcept -> DirectX::XMVECTOR
		{
			return DirectX::XMPlaneFromPoints(
				DirectX::XMLoadFloat3(&V0),
				DirectX::XMLoadFloat3(&V1),
				DirectX::XMLoadFloat3(&V2)
			);
		}

		auto GetNormalAsVector() const noexcept -> DirectX::XMVECTOR
		{
			auto v0 = DirectX::XMLoadFloat3(&V0);
			auto v1 = DirectX::XMLoadFloat3(&V1);
			auto v2 = DirectX::XMLoadFloat3(&V2);
			return DirectX::XMPlaneFromPoints(v0, v1, v2);
		}

		auto GetNormalAsFloat3() const noexcept -> DirectX::XMFLOAT3
		{
			auto v0 = DirectX::XMLoadFloat3(&V0);
			auto v1 = DirectX::XMLoadFloat3(&V1);
			auto v2 = DirectX::XMLoadFloat3(&V2);
			auto normal = DirectX::XMPlaneFromPoints(v0, v1, v2);
			auto result = DirectX::XMFLOAT3{};
			DirectX::XMStoreFloat3(&result, normal);
			return result;
		}

		auto __vectorcall operator-=(DirectX::XMVECTOR translation) -> Triangle&
		{
			auto v0 = DirectX::XMLoadFloat3(&V0);
			auto v1 = DirectX::XMLoadFloat3(&V1);
			auto v2 = DirectX::XMLoadFloat3(&V2);
			v0 -= translation;
			v1 -= translation;
			v2 -= translation;
			DirectX::XMStoreFloat3(&V0, v0);
			DirectX::XMStoreFloat3(&V1, v1);
			DirectX::XMStoreFloat3(&V2, v2);
			return *this;
		}

		auto __vectorcall operator+=(DirectX::XMVECTOR translation) -> Triangle&
		{
			auto v0 = DirectX::XMLoadFloat3(&V0);
			auto v1 = DirectX::XMLoadFloat3(&V1);
			auto v2 = DirectX::XMLoadFloat3(&V2);
			v0 += translation;
			v1 += translation;
			v2 += translation;
			DirectX::XMStoreFloat3(&V0, v0);
			DirectX::XMStoreFloat3(&V1, v1);
			DirectX::XMStoreFloat3(&V2, v2);
			return *this;
		}
	};

	/*
	* Sphere triangle collision
	* 1. Translate the triangle to the sphere's local space by 
		subtracting the sphere's center from each vertex of the triangle.
	* 2. Compute the plane of the triangle using the cross product of two edges.
	* 3. Project the sphere's center onto the plane of the triangle. This is
	*	done by first normalizing the plane's normal vector, the scaling the 
	*	normal by the adjusted D value.
	* 4. Check if the projected point is inside the triangle using barycentric coordinates.
	*/
}