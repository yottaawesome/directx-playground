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
	// A limitation is here is that it return the projected points on the plane if the
	// point is not in the triangle.
	auto PointToBarycentricCoordinates(
		const DirectX::XMFLOAT3& point,
		const DirectX::XMFLOAT3& a,
		const DirectX::XMFLOAT3& b,
		const DirectX::XMFLOAT3& c
	) noexcept -> DirectX::XMFLOAT3
	{
		auto v0 = DirectX::XMLoadFloat3(&b) - DirectX::XMLoadFloat3(&a);
		auto v1 = DirectX::XMLoadFloat3(&c) - DirectX::XMLoadFloat3(&a);
		auto v2 = DirectX::XMLoadFloat3(&point) - DirectX::XMLoadFloat3(&a);
		auto d00 = Dot3(v0, v0);
		auto d01 = Dot3(v0, v1);
		auto d11 = Dot3(v1, v1);
		auto d20 = Dot3(v2, v0);
		auto d21 = Dot3(v2, v1);
		auto denom = d00 * d11 - d01 * d01;
		if (denom == 0.0f)
			return { -1.0f, -1.0f, -1.0f }; // Degenerate triangle
		auto v = (d11 * d20 - d01 * d21) / denom;
		auto w = (d00 * d21 - d01 * d20) / denom;
		auto u = 1.0f - v - w;
		return { u, v, w };
	}

	// Returns weights for {a, b, c}; valid nonnegative weights identify points inside/on the triangle.
	// Coplanarity and triangle altitude use a tolerance of 1e-5 times the longest edge.
	// Returns {-1, -1, -1} for non-finite inputs, degenerate/nearly degenerate triangles,
	// off-plane points beyond that tolerance, or weights outside the finite float range.
	auto PointToBarycentricCoordinates2(
		const DirectX::XMFLOAT3& point,
		const DirectX::XMFLOAT3& a,
		const DirectX::XMFLOAT3& b,
		const DirectX::XMFLOAT3& c
	) noexcept -> DirectX::XMFLOAT3
	{
		constexpr auto InvalidCoordinates = DirectX::XMFLOAT3{ -1.0f, -1.0f, -1.0f };
		constexpr auto RelativeTolerance = 1e-5;
		constexpr auto ToleranceSquared = RelativeTolerance * RelativeTolerance;
		if (not IsFinite(point) or not IsFinite(a) or not IsFinite(b) or not IsFinite(c))
			return InvalidCoordinates;

		// Promote before subtracting, so edge/offset arithmetic does not overflow or round in float.
		using Double3 = std::array<double, 3>;
		const auto subtract = 
			[](const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
			{
				return Double3{
					static_cast<double>(lhs.x) - rhs.x,
					static_cast<double>(lhs.y) - rhs.y,
					static_cast<double>(lhs.z) - rhs.z
				};
			};
		const auto dot = 
			[](const Double3& lhs, const Double3& rhs)
			{
				return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
			};
		const auto cross = 
			[](const Double3& lhs, const Double3& rhs)
			{
				return Double3{
					lhs[1] * rhs[2] - lhs[2] * rhs[1],
					lhs[2] * rhs[0] - lhs[0] * rhs[2],
					lhs[0] * rhs[1] - lhs[1] * rhs[0]
				};
			};
		const auto ab = subtract(b, a);
		const auto ac = subtract(c, a);
		const auto bc = subtract(c, b);
		const auto offset = subtract(point, a);
		const auto longestEdgeSquared = std::max({ dot(ab, ab), dot(ac, ac), dot(bc, bc) });

		// The squared cross-product magnitude avoids subtracting nearly equal Gram products.
		const auto normal = cross(ab, ac);
		const auto normalSquared = dot(normal, normal);
		// Reject zero area and an altitude <= 1e-5 of the longest edge, regardless of scale/winding.
		if (normalSquared <= ToleranceSquared * longestEdgeSquared * longestEdgeSquared)
			return InvalidCoordinates;

		// Plane distance is |offset dot normal| / |normal|; compare squared distances without normalizing.
		const auto planeOffset = dot(offset, normal);
		if (planeOffset * planeOffset > ToleranceSquared * longestEdgeSquared * normalSquared)
			return InvalidCoordinates;

		// Signed area ratios give the weights for b and c; the weight for a makes their sum one.
		const auto v = dot(cross(offset, ac), normal) / normalSquared;
		const auto w = dot(cross(ab, offset), normal) / normalSquared;
		const auto u = 1.0 - v - w;
		// Very distant coplanar points can require weights too large for the XMFLOAT3 return type.
		constexpr auto MaxFloat = static_cast<double>(std::numeric_limits<float>::max());
		if (std::fabs(u) > MaxFloat or std::fabs(v) > MaxFloat or std::fabs(w) > MaxFloat)
			return InvalidCoordinates;

		return { static_cast<float>(u), static_cast<float>(v), static_cast<float>(w) };
	}

	// Calculates barycentric weights for point P relative to triangle ABC using SIMD registers.
	// Employs the recommended DirectXMath calling convention for stack/register allocation.
	// Generated by Gemini, as I wanted to see how it would do it compared to other methods.
	auto GetBarycentric3D(
		DirectX::XMVECTOR P,
		DirectX::XMVECTOR A,
		DirectX::XMVECTOR B,
		DirectX::XMVECTOR C,
		float& outWA, 
		float& outWB, 
		float& outWC
	) -> bool
	{
		// Triangle base vectors
		auto v0 = DirectX::XMVectorSubtract(B, A);
		auto v1 = DirectX::XMVectorSubtract(C, A);

		// Main triangle normal vector
		auto n = DirectX::XMVector3Cross(v0, v1);

		// Compute d = dot(n, n). XMVector3Dot returns the result replicated across all 4 components.
		auto d_vec = DirectX::XMVector3Dot(n, n);
		auto d = float{};
		DirectX::XMStoreFloat(&d, d_vec);

		// Guard against degenerate (zero-area) triangles
		if (std::abs(d) < 1e-12f) 
		{
			outWA = outWB = outWC = 0.0f;
			return false;
		}

		// Compute sub-triangle normals using relative vector offsets
		auto nA = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(C, B), DirectX::XMVectorSubtract(P, B));
		auto nB = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(A, C), DirectX::XMVectorSubtract(P, C));
		auto nC = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(P, A));

		// Compute dot products. Results are vector SIMD registers containing [dot, dot, dot, dot]
		auto dotA = DirectX::XMVector3Dot(n, nA);
		auto dotB = DirectX::XMVector3Dot(n, nB);
		auto dotC = DirectX::XMVector3Dot(n, nC);

		// Unpack the scalar float outputs from the SIMD registers and divide by the squared total normal length
		auto scalarDotA = float{};
		auto scalarDotB = float{};
		auto scalarDotC = float{};
		DirectX::XMStoreFloat(&scalarDotA, dotA);
		DirectX::XMStoreFloat(&scalarDotB, dotB);
		DirectX::XMStoreFloat(&scalarDotC, dotC);

		outWA = scalarDotA / d;
		outWB = scalarDotB / d;
		outWC = scalarDotC / d;

		return true;
	}

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