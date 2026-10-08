import std;
import Shared;

struct Triangle
{
	DirectX::XMFLOAT3 V0{0, 0, 0};
	DirectX::XMFLOAT3 V1{0, 0, 0};
	DirectX::XMFLOAT3 V2{0, 0, 0};

	auto GetPlaneAsFloat3() const noexcept -> DirectX::XMFLOAT4
	{
		auto vector = DirectX::XMVECTOR{
			DirectX::XMPlaneFromPoints(
				DirectX::XMLoadFloat3(&V0),
				DirectX::XMLoadFloat3(&V1),
				DirectX::XMLoadFloat3(&V2)
			)};
		auto result = DirectX::XMFLOAT4{};
		DirectX::XMStoreFloat4(&result, vector);
		return result;
	}

	// XMVECTOR is fine here. XMVECTOR in class members
	auto GetPlaneAsVector() const noexcept -> DirectX::XMVECTOR
	{
		return DirectX::XMPlaneFromPoints(
			DirectX::XMLoadFloat3(&V0),
			DirectX::XMLoadFloat3(&V1),
			DirectX::XMLoadFloat3(&V2)
		);
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

struct Sphere
{
	DirectX::XMFLOAT3 Center{ 0, 0, 0 };
	float Radius{ 0 };
};

namespace
{
	auto IsFinite(const DirectX::XMFLOAT3& point) noexcept -> bool
	{
		return std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z);
	}

	auto Dot3(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) noexcept -> float
	{
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(lhs, rhs));
	}

	auto IsProjectionInsideTriangle(
		const DirectX::XMVECTOR& point,
		const DirectX::XMVECTOR& a, 
		const DirectX::XMVECTOR& b,
		const DirectX::XMVECTOR& c, 
		const DirectX::XMVECTOR& normal
	) noexcept -> bool
	{
		// A point's plane projection is inside/on the triangle iff
		// N dot ((end - start) cross (point - start)) >= 0 for edges AB, BC, CA.
		// A component parallel to N does not affect these signs. Reversing the
		// winding reverses both N and the edges, so this test is two-sided.
		return
			Dot3(normal, DirectX::XMVector3Cross(b - a, point - a)) >= 0.0f &&
			Dot3(normal, DirectX::XMVector3Cross(c - b, point - b)) >= 0.0f &&
			Dot3(normal, DirectX::XMVector3Cross(a - c, point - c)) >= 0.0f;
	}
}

auto DetectCollision(const Triangle& triangle, const Sphere& sphere) -> bool
{
	if (not std::isfinite(sphere.Radius) 
		or not sphere.Radius < 0.0f 
		or not IsFinite(sphere.Center) 
		or not IsFinite(triangle.V0) 
		or not IsFinite(triangle.V1) 
		or not IsFinite(triangle.V2))
	{
		throw std::invalid_argument{"Sphere-triangle collision requires finite coordinates and a finite, nonnegative radius."};
	}

	const auto a = DirectX::XMLoadFloat3(&triangle.V0);
	const auto b = DirectX::XMLoadFloat3(&triangle.V1);
	const auto c = DirectX::XMLoadFloat3(&triangle.V2);
	const auto center = DirectX::XMLoadFloat3(&sphere.Center);
	const auto radiusSquared = sphere.Radius * sphere.Radius;
	// Intersection means min_{X in triangle} ||center - X||^2 <= radius^2.
	// This is a two-sided test, and equality includes tangential contact.
	const auto ab = b - a;
	const auto normal = DirectX::XMVector3Cross(ab, c - a);
	// 𝐯⋅𝐯 = ‖𝐯‖^2
	const auto normalSquared = Dot3(normal, normal);
	if (normalSquared > 0.0f)
	{
		// The plane equation is N dot (X - A) = 0, with N = (B - A) cross (C - A).
		// Project center P onto it: Q = P - N * (N dot (P - A)) / (N dot N).
		// N need not be normalized, and using P - A accounts for the plane's offset.
		const auto projection = center - DirectX::XMVectorScale(normal, Dot3(normal, center - a) / normalSquared);

		if (IsProjectionInsideTriangle(projection, a, b, c, normal))
		{
			// An interior projection is the closest point on the whole triangle.
			const auto separation = center - projection;
			return Dot3(separation, separation) <= radiusSquared;
		}
	}

	// An exterior projection has its closest point on an edge or vertex.
	// A degenerate triangle has no face, so its segments/points also use this path.
	const auto segmentDistanceSquared = 
		[&](const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end)
		{
			const auto edge = end - start;
			const auto edgeSquared = Dot3(edge, edge);

			// Minimize ||P - (start + t * edge)||^2 for 0 <= t <= 1:
			// t = clamp(((P - start) dot edge) / (edge dot edge), 0, 1).
			// Clamping includes the endpoints; a zero-length segment is just start.
			const auto t = edgeSquared > 0.0f 
				? std::clamp(Dot3(center - start, edge) / edgeSquared, 0.0f, 1.0f) 
				: 0.0f;
			const auto closestPoint = start + DirectX::XMVectorScale(edge, t);
			const auto separation = center - closestPoint;
			return Dot3(separation, separation);
		};
	const auto distanceSquared = std::min({
		segmentDistanceSquared(a, b),
		segmentDistanceSquared(b, c),
		segmentDistanceSquared(c, a)
	});
	return distanceSquared <= radiusSquared;
}

auto DetectCollision(
	const Triangle& triangle, 
	const Sphere& sphere,
	const DirectX::XMFLOAT3& displacement
) -> bool
{
	if (not IsFinite(displacement))
		throw std::invalid_argument("Sphere displacement must contain finite coordinates.");

	// The vector is a full displacement, not a unit direction: P(t) = P0 + t * D,
	// where P0 is the initial sphere center and t is the movement fraction,
	// with 0 <= t <= 1. Do not normalize D, since its length sets the travel distance.
	// The swept sphere is a capsule and intersects iff
	// min_{0 <= t <= 1, X in triangle} ||P0 + t * D - X||^2 <= radius^2.
	if (DetectCollision(triangle, sphere))
		// This also validates the sphere/triangle and includes contact at t = 0,
		// even when the sphere is moving away from an existing overlap.
		return true;

	const auto center = DirectX::XMLoadFloat3(&sphere.Center);
	const auto movement = DirectX::XMLoadFloat3(&displacement);
	const auto movementSquared = Dot3(movement, movement);
	if (movementSquared == 0.0f)
		// With no movement, the initial static result is the entire result.
		return false;

	auto destination = sphere;
	DirectX::XMStoreFloat3(&destination.Center, center + movement);
	if (DetectCollision(triangle, destination))
		return true;

	const auto a = DirectX::XMLoadFloat3(&triangle.V0);
	const auto b = DirectX::XMLoadFloat3(&triangle.V1);
	const auto c = DirectX::XMLoadFloat3(&triangle.V2);
	const auto radiusSquared = sphere.Radius * sphere.Radius;
	const auto normal = DirectX::XMVector3Cross(b - a, c - a);
	const auto normalSquared = Dot3(normal, normal);
	const auto normalMovement = Dot3(normal, movement);
	if (normalSquared > 0.0f and normalMovement != 0.0f)
	{
		// A center-path crossing of the triangle itself has distance zero.
		// Solve N dot (P0 + t * D - A) = 0:
		// t = (N dot (A - P0)) / (N dot D), restricted to the finite path [0, 1].
		const auto t = Dot3(normal, a - center) / normalMovement;
		if (t >= 0.0f and t <= 1.0f)
		{
			const auto crossing = center + DirectX::XMVectorScale(movement, t);
			if (IsProjectionInsideTriangle(crossing, a, b, c, normal))
				return true;
		}
	}

	// Otherwise the minimum is at a path endpoint (already tested) or a triangle
	// edge. If the path is parallel to the face, an equally close pair exists at
	// an endpoint or where its projection enters an edge. Thus these edge tests
	// also cover parallel face contacts, collinear triangles, and vertices.
	const auto segmentDistanceSquared = 
		[&](const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end)
		{
			const auto edge = end - start;
			const auto offset = center - start;
			const auto edgeSquared = Dot3(edge, edge);
			const auto movementDotOffset = Dot3(movement, offset);
			auto t = 0.0f;
			auto u = 0.0f;
			if (edgeSquared == 0.0f)
			{
				// A collapsed edge is a point: minimize ||offset + t * movement||^2.
				t = std::clamp(-movementDotOffset / movementSquared, 0.0f, 1.0f);
			}
			else
			{
				const auto movementDotEdge = Dot3(movement, edge);
				const auto edgeDotOffset = Dot3(edge, offset);

				// Minimize ||R + t * D - u * E||^2, where R = P0 - start.
				// With a = D dot D, b = D dot E, c = E dot E,
				// d = D dot R, e = E dot R, the stationary equations are
				// a*t - b*u = -d and c*u - b*t = e.
				// Hence t = (b*e - c*d) / (a*c - b*b).
				const auto cross = DirectX::XMVector3Cross(movement, edge);
				const auto denominator = Dot3(cross, cross);
				if (denominator > 0.0f)
				{
					// Equivalent cross products avoid subtracting nearly equal a*c
					// and b*b for almost-parallel segments:
					// t = ((E cross R) dot (D cross E)) / ||D cross E||^2.
					t = std::clamp(Dot3(DirectX::XMVector3Cross(edge, offset), cross) /
						denominator, 0.0f, 1.0f);
				}

				// For parallel segments, start with t = 0. For either case,
				// u = (b*t + e) / c. If u lies outside the edge, fix it to that
				// endpoint and minimize again: t = clamp((b*u - d) / a, 0, 1).
				u = (movementDotEdge * t + edgeDotOffset) / edgeSquared;
				if (u < 0.0f)
				{
					u = 0.0f;
					t = std::clamp(-movementDotOffset / movementSquared, 0.0f, 1.0f);
				}
				else if (u > 1.0f)
				{
					u = 1.0f;
					t = std::clamp((movementDotEdge - movementDotOffset) /
						movementSquared, 0.0f, 1.0f);
				}
			}
			const auto separation = 
				offset 
				+ DirectX::XMVectorScale(movement, t) 
				- DirectX::XMVectorScale(edge, u);
			return Dot3(separation, separation);
		};
	const auto distanceSquared = std::min({
		segmentDistanceSquared(a, b),
		segmentDistanceSquared(b, c),
		segmentDistanceSquared(c, a)
	});
	return distanceSquared <= radiusSquared;
}

namespace DetectCollisionTests
{
	void TestStaticOverlaps()
	{
		const auto expect = 
			[](
				std::string_view name, 
				const Triangle& triangle,
				const Sphere& sphere, 
				bool expected
			)
			{
				if (DetectCollision(triangle, sphere) != expected 
					or DetectCollision(triangle, sphere, DirectX::XMFLOAT3{ 0, 0, 0 }) != expected)
				{
					throw std::runtime_error(std::string(name) + ": unexpected collision result.");
				}
			};
		const auto triangle = Triangle{
			DirectX::XMFLOAT3{ 0, 0, 0 },
			DirectX::XMFLOAT3{ 1, 0, 0 },
			DirectX::XMFLOAT3{ 0, 1, 0 }
		};
		const auto reversed = Triangle{ triangle.V0, triangle.V2, triangle.V1 };
		struct TestCase
		{
			std::string_view Name;
			Sphere Ball;
			bool Expected;
		};
		const auto cases = std::array{
			TestCase{ "Face contact above", Sphere{ { 0.25f, 0.25f, 0.5f }, 0.5f }, true },
			TestCase{ "Face contact below", Sphere{ { 0.25f, 0.25f, -0.5f }, 0.5f }, true },
			TestCase{ "Face separation", Sphere{ { 0.25f, 0.25f, 0.51f }, 0.5f }, false },
			TestCase{ "Infinite-plane false positive", Sphere{ { 100, 100, 0 }, 1 }, false },
			TestCase{ "AB edge contact", Sphere{ { 0.5f, -0.5f, 0 }, 0.5f }, true },
			TestCase{ "AB edge separation", Sphere{ { 0.5f, -0.51f, 0 }, 0.5f }, false },
			TestCase{ "BC edge overlap", Sphere{ { 0.75f, 0.75f, 0 }, 0.4f }, true },
			TestCase{ "BC edge separation", Sphere{ { 1, 1, 0 }, 0.5f }, false },
			TestCase{ "CA edge contact", Sphere{ { -0.5f, 0.5f, 0 }, 0.5f }, true },
			TestCase{ "A vertex contact", Sphere{ { -1, 0, 0 }, 1 }, true },
			TestCase{ "A vertex separation", Sphere{ { -1.01f, 0, 0 }, 1 }, false },
			TestCase{ "B vertex contact", Sphere{ { 2, 0, 0 }, 1 }, true },
			TestCase{ "C vertex contact", Sphere{ { 0, 2, 0 }, 1 }, true },
			TestCase{ "Zero-radius face contact", Sphere{ { 0.25f, 0.25f, 0 }, 0 }, true },
			TestCase{ "Zero-radius separation", Sphere{ { 0.25f, 0.25f, 0.01f }, 0 }, false },
			TestCase{ "Zero-radius vertex contact", Sphere{ { 0, 0, 0 }, 0 }, true }
		};
		for (const auto& test : cases)
		{
			expect(test.Name, triangle, test.Ball, test.Expected);
			expect(test.Name, reversed, test.Ball, test.Expected);
		}

		const auto translated = Triangle{
			DirectX::XMFLOAT3{ 2, 3, 5 },
			DirectX::XMFLOAT3{ 3, 3, 5 },
			DirectX::XMFLOAT3{ 2, 4, 5 }
		};
		expect("Translated plane contact", translated, Sphere{ { 2.25f, 3.25f, 5.5f }, 0.5f }, true);
		expect("Translated plane separation", translated, Sphere{ { 2.25f, 3.25f, 0 }, 0.5f }, false);

		const auto tilted = Triangle{
			DirectX::XMFLOAT3{ 0, 0, 0 },
			DirectX::XMFLOAT3{ 1, 1, 0 },
			DirectX::XMFLOAT3{ 0, 1, 1 }
		};
		expect("Tilted face overlap", tilted, Sphere{ { 0.75f, 0, 0.75f }, 1 }, true);
		expect("Tilted face separation", tilted, Sphere{ { 0.75f, 0, 0.75f }, 0.5f }, false);
		expect("Tilted edge contact", tilted, Sphere{ { 0.5f, 0.5f, -0.5f }, 0.5f }, true);
		expect("Tilted plane false positive", tilted, Sphere{ { 10, 20, 10 }, 1 }, false);

		const auto line = Triangle{
			DirectX::XMFLOAT3{ 0, 0, 0 },
			DirectX::XMFLOAT3{ 1, 0, 0 },
			DirectX::XMFLOAT3{ 2, 0, 0 }
		};
		expect("Collinear contact", line, Sphere{ { 1, 0.5f, 0 }, 0.5f }, true);
		expect("Collinear separation", line, Sphere{ { 1, 0.51f, 0 }, 0.5f }, false);
		const auto repeatedVertex = Triangle{ line.V0, line.V0, line.V2 };
		expect("Repeated vertex contact", repeatedVertex, Sphere{ { 1, 0.5f, 0 }, 0.5f }, true);
		const auto point = Triangle{
			DirectX::XMFLOAT3{ 2, 3, 4 },
			DirectX::XMFLOAT3{ 2, 3, 4 },
			DirectX::XMFLOAT3{ 2, 3, 4 }
		};
		expect("Point contact", point, Sphere{ { 2, 3, 5 }, 1 }, true);
		expect("Point separation", point, Sphere{ { 2, 3, 5.01f }, 1 }, false);

		const auto expectInvalid = [](const Triangle& shape, const Sphere& ball)
		{
			try
			{
				DetectCollision(shape, ball);
			}
			catch (const std::invalid_argument&)
			{
				return;
			}
			throw std::runtime_error("Invalid collision input was accepted.");
		};
		expectInvalid(triangle, Sphere{ { 0, 0, 0 }, -1 });
		expectInvalid(triangle, Sphere{ { 0, 0, 0 }, std::numeric_limits<float>::infinity() });
		expectInvalid(triangle, Sphere{ { std::numeric_limits<float>::quiet_NaN(), 0, 0 }, 1 });
		auto invalidTriangle = triangle;
		invalidTriangle.V0.x = std::numeric_limits<float>::infinity();
		expectInvalid(invalidTriangle, Sphere{ { 0, 0, 0 }, 1 });
	}

	void TestSweptOverlaps()
	{
		const auto expect = [](std::string_view name, const Triangle& triangle,
			const Sphere& sphere, const DirectX::XMFLOAT3& displacement, bool expected)
		{
			if (DetectCollision(triangle, sphere, displacement) != expected)
			{
				throw std::runtime_error(std::string(name) + ": unexpected swept collision result.");
			}
		};
		const auto triangle = Triangle{
			DirectX::XMFLOAT3{ 0, 0, 0 },
			DirectX::XMFLOAT3{ 1, 0, 0 },
			DirectX::XMFLOAT3{ 0, 1, 0 }
		};
		const auto reversed = Triangle{ triangle.V0, triangle.V2, triangle.V1 };
		const auto translated = Triangle{
			DirectX::XMFLOAT3{ 2, 3, 5 },
			DirectX::XMFLOAT3{ 3, 3, 5 },
			DirectX::XMFLOAT3{ 2, 4, 5 }
		};
		struct TestCase
		{
			std::string_view Name;
			Sphere Ball;
			DirectX::XMFLOAT3 Displacement;
			bool Expected;
		};
		const auto cases = std::array{
			TestCase{ "Face crossing above", Sphere{ { 0.25f, 0.25f, 2 }, 0.25f }, { 0, 0, -4 }, true },
			TestCase{ "Face crossing below", Sphere{ { 0.25f, 0.25f, -2 }, 0.25f }, { 0, 0, 4 }, true },
			TestCase{ "Movement stops short", Sphere{ { 0.25f, 0.25f, 2 }, 0.25f }, { 0, 0, -1 }, false },
			TestCase{ "Final face contact", Sphere{ { 0.25f, 0.25f, 2 }, 0.25f }, { 0, 0, -1.75f }, true },
			TestCase{ "Moving away", Sphere{ { 0.25f, 0.25f, 2 }, 0.25f }, { 0, 0, 4 }, false },
			TestCase{ "Initial overlap", Sphere{ { 0.25f, 0.25f, 0 }, 0.25f }, { 0, 0, 4 }, true },
			TestCase{ "Parallel face contact", Sphere{ { -1, 0.25f, 0.5f }, 0.5f }, { 3, 0, 0 }, true },
			TestCase{ "Parallel face separation", Sphere{ { -1, 0.25f, 0.51f }, 0.5f }, { 3, 0, 0 }, false },
			TestCase{ "Plane crossing outside", Sphere{ { 2, 2, 2 }, 0.25f }, { 0, 0, -4 }, false },
			TestCase{ "AB edge crossing", Sphere{ { 0.5f, -0.2f, 2 }, 0.25f }, { 0, 0, -4 }, true },
			TestCase{ "AB edge miss", Sphere{ { 0.5f, -0.26f, 2 }, 0.25f }, { 0, 0, -4 }, false },
			TestCase{ "BC edge crossing", Sphere{ { 0.75f, 0.75f, 2 }, 0.4f }, { 0, 0, -4 }, true },
			TestCase{ "BC edge miss", Sphere{ { 0.75f, 0.75f, 2 }, 0.3f }, { 0, 0, -4 }, false },
			TestCase{ "CA edge crossing", Sphere{ { -0.2f, 0.5f, 2 }, 0.25f }, { 0, 0, -4 }, true },
			TestCase{ "A vertex tangency", Sphere{ { -0.5f, 0, 2 }, 0.5f }, { 0, 0, -4 }, true },
			TestCase{ "A vertex miss", Sphere{ { -0.51f, 0, 2 }, 0.5f }, { 0, 0, -4 }, false },
			TestCase{ "B vertex tangency", Sphere{ { 1.5f, 0, 2 }, 0.5f }, { 0, 0, -4 }, true },
			TestCase{ "C vertex tangency", Sphere{ { 0, 1.5f, 2 }, 0.5f }, { 0, 0, -4 }, true },
			TestCase{ "Zero-radius face crossing", Sphere{ { 0.25f, 0.25f, 2 }, 0 }, { 0, 0, -4 }, true },
			TestCase{ "Zero-radius edge crossing", Sphere{ { 0.5f, 0, 2 }, 0 }, { 0, 0, -4 }, true },
			TestCase{ "Zero-radius vertex crossing", Sphere{ { 0, 0, 2 }, 0 }, { 0, 0, -4 }, true },
			TestCase{ "Zero-radius miss", Sphere{ { 1, 1, 2 }, 0 }, { 0, 0, -4 }, false },
			TestCase{ "Coplanar crossing", Sphere{ { -1, 0.25f, 0 }, 0 }, { 3, 0, 0 }, true },
			TestCase{ "Coplanar miss", Sphere{ { -1, 2, 0 }, 0.25f }, { 3, 0, 0 }, false },
			TestCase{ "Parallel edge tangency", Sphere{ { -2, -0.5f, 0 }, 0.5f }, { 5, 0, 0 }, true }
		};
		for (const auto& test : cases)
		{
			expect(test.Name, triangle, test.Ball, test.Displacement, test.Expected);
			expect(test.Name, reversed, test.Ball, test.Displacement, test.Expected);
			auto shifted = test.Ball;
			shifted.Center.x += 2;
			shifted.Center.y += 3;
			shifted.Center.z += 5;
			expect(test.Name, translated, shifted, test.Displacement, test.Expected);
		}

		const auto tilted = Triangle{
			DirectX::XMFLOAT3{ 0, 0, 0 },
			DirectX::XMFLOAT3{ 1, 1, 0 },
			DirectX::XMFLOAT3{ 0, 1, 1 }
		};
		expect("Tilted face crossing", tilted, Sphere{ { 0.75f, 0, 0.75f }, 0.1f }, { -1, 1, -1 }, true);
		const auto line = Triangle{
			DirectX::XMFLOAT3{ 0, 0, 0 },
			DirectX::XMFLOAT3{ 1, 0, 0 },
			DirectX::XMFLOAT3{ 2, 0, 0 }
		};
		expect("Swept collinear contact", line, Sphere{ { 1, 0.5f, 2 }, 0.5f }, { 0, 0, -4 }, true);
		expect("Swept collinear miss", line, Sphere{ { 1, 0.51f, 2 }, 0.5f }, { 0, 0, -4 }, false);
		expect("Edge extension miss", line, Sphere{ { -1, -1, 0 }, 0.5f }, { 0, 2, 0 }, false);
		expect("Edge endpoint tangency", line, Sphere{ { -1, -1, 0 }, 1 }, { 0, 2, 0 }, true);
		const auto repeatedVertex = Triangle{ line.V0, line.V0, line.V2 };
		expect("Swept repeated vertex", repeatedVertex, Sphere{ { 1, 0.5f, 2 }, 0.5f }, { 0, 0, -4 }, true);
		const auto almostParallel = Triangle{
			DirectX::XMFLOAT3{ 0, 0, 0 },
			DirectX::XMFLOAT3{ 10000, 1, 0 },
			DirectX::XMFLOAT3{ 20000, 2, 0 }
		};
		expect("Almost-parallel crossing", almostParallel, Sphere{ { 0, 0.5f, 0 }, 0 },
			{ 10000, 0, 0 }, true);
		const auto point = Triangle{
			DirectX::XMFLOAT3{ 2, 3, 4 },
			DirectX::XMFLOAT3{ 2, 3, 4 },
			DirectX::XMFLOAT3{ 2, 3, 4 }
		};
		expect("Swept point crossing", point, Sphere{ { 2, 3, 2 }, 0 }, { 0, 0, 4 }, true);
		expect("Swept point tangency", point, Sphere{ { 2.5f, 3, 2 }, 0.5f }, { 0, 0, 4 }, true);
		expect("Swept point miss", point, Sphere{ { 2.51f, 3, 2 }, 0.5f }, { 0, 0, 4 }, false);

		const auto overlapping = Sphere{ { 0.25f, 0.25f, 0 }, 1 };
		for (const auto& invalid : std::array{
			DirectX::XMFLOAT3{ std::numeric_limits<float>::quiet_NaN(), 0, 0 },
			DirectX::XMFLOAT3{ 0, std::numeric_limits<float>::infinity(), 0 },
			DirectX::XMFLOAT3{ 0, 0, -std::numeric_limits<float>::infinity() }
		})
		{
			try
			{
				DetectCollision(triangle, overlapping, invalid);
			}
			catch (const std::invalid_argument&)
			{
				continue;
			}
			throw std::runtime_error("Invalid sphere displacement was accepted.");
		}
	}

	void TestOneHit()
	{
		auto triangle = Triangle{
			DirectX::XMFLOAT3{ 0, 0, 0 },
			DirectX::XMFLOAT3{ 1, 0, 0 },
			DirectX::XMFLOAT3{ 0, 1, 0 }
		};
		auto sphere = Sphere{
			DirectX::XMFLOAT3{ 0.5f, 0.5f, -1.0f },
			1.0f
		};
		std::cout << (DetectCollision(triangle, sphere) ? "Collision detected!" : "No collision.")
			<< std::endl;
	}
}

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	DetectCollisionTests::TestStaticOverlaps();
	DetectCollisionTests::TestSweptOverlaps();
	DetectCollisionTests::TestOneHit();
	return 0;
}