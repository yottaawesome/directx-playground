export module Shared:Math;
import std;
import Win32;
import DirectXMath;

export namespace Math
{
	using Quaternion = DirectX::XMFLOAT4;

	auto IsFinite(const DirectX::XMFLOAT3& point) noexcept -> bool
	{
		return std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z);
	}

	auto Dot3(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) noexcept -> float
	{
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(lhs, rhs));
	}

	auto NearEqual(float a, float b, float tolerance = 0.00001f) noexcept -> bool
	{
		return std::fabs(a - b) <= tolerance;
	}
}

export namespace Math
{
	struct Sphere
	{
		DirectX::XMFLOAT3 Center{ 0, 0, 0 };
		float Radius{ 0 };
	};

	struct SphereCompact
	{
		DirectX::XMFLOAT4 Data{ 0, 0, 0, 0 };
		DirectX::XMFLOAT4 Orientation{ 0, 0, 0, 1 };

		auto Rotate(float x, float y, float z) const noexcept -> SphereCompact
		{
			Quaternion rotation{ x, y, z, 1 };

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
			std::size_t seed = 0;
			for (float value : { plane.Normal.x, plane.Normal.y, plane.Normal.z, plane.D })
			{
				seed ^= std::hash<float>{}(value)+0x9e3779b9u
					+ (seed << 6) + (seed >> 2);
			}
			return seed;
		}
	};

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

		auto __vectorcall operator-=(DirectX::FXMVECTOR translation) -> Triangle&
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

		auto __vectorcall operator+=(DirectX::FXMVECTOR translation) -> Triangle&
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