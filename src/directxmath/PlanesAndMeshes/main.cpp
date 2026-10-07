import std;
import Shared;
import PlanesAndMeshes;

[[nodiscard]]
auto LoadFile() -> std::vector<PlanesAndMeshes::Physics::Triangle>
{
	constexpr auto FilePath = "mesh.txt";

	auto inputFile = std::ifstream{ FilePath };
	if (inputFile.fail())
		throw std::runtime_error{ std::format("Failed to open {}", FilePath) };

	struct ReadVertices {};
	struct ReadIndices {};
	auto state = Util::Variant<ReadVertices, ReadIndices>{ };
	auto line = std::string{};
	auto float3s = std::vector<DirectX::XMFLOAT3>{};
	auto indices = std::vector<size_t>{};
	while (std::getline(inputFile, line))
	{
		if (line.empty())
		{
			continue;
		}
		else if (line.starts_with("Vertices:"))
		{
			state = ReadVertices{};
			continue;
		}
		else if (line.starts_with("Indices:"))
		{
			state = ReadIndices{};
			continue;
		}

		state(
			[&](ReadVertices) 
			{
				auto parts = String::TokeniseString(line, " ");
				if (parts.size() != 3)
					throw std::runtime_error{ std::format("Expected 3 values, got {}", parts.size()) };
				float3s.emplace_back(std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]));
			},
			[&](ReadIndices) 
			{
				auto index = size_t{};
				auto [end, error] = std::from_chars(line.data(), line.data() + line.size(), index);
				if (error != std::errc{} or end != line.data() + line.size())
					throw std::runtime_error{ std::format("Invalid mesh index '{}'", line) };
				if (index >= float3s.size())
					throw std::runtime_error{ std::format("Mesh index {} is out of range for {} vertices", index, float3s.size()) };
				indices.push_back(index);
			});
	}

	if (indices.size() % 3 != 0)
		throw std::runtime_error{ std::format("Expected index count to be divisible by 3, got {}", indices.size()) };

	auto triples = indices | std::ranges::views::chunk(3);
	auto returnValue = std::vector<PlanesAndMeshes::Physics::Triangle>{};
	returnValue.reserve(indices.size() / 3);
	for (auto chunk : triples)
	{
		returnValue.push_back(PlanesAndMeshes::Physics::Triangle{ float3s[chunk[0]], float3s[chunk[1]], float3s[chunk[2]] });
	}

	return returnValue;
}

using PlaneGroup = std::pair<PlanesAndMeshes::Physics::Plane, std::vector<PlanesAndMeshes::Physics::Triangle>>;

[[nodiscard]]
auto GroupTrianglesByPlane(const std::vector<PlanesAndMeshes::Physics::Triangle>& triangles) -> std::vector<PlaneGroup>
{
	constexpr auto PlaneTolerance = 1e-5f;
	auto planeGroups = std::vector<PlaneGroup>{};
	for (const PlanesAndMeshes::Physics::Triangle& triangle : triangles)
	{
		auto plane = triangle.GetPlane();
		// Normalize +0/-0 to 0 
		if (plane.Normal.x == 0.0f)
			plane.Normal.x = 0;
		if (plane.Normal.y == 0.0f)
			plane.Normal.y = 0;
		if (plane.Normal.z == 0.0f)
			plane.Normal.z = 0;

		// Tolerance matching is not transitive; keep each group's first plane as its representative.
		auto group = std::ranges::find_if(
			planeGroups, 
			[&](const PlaneGroup& candidate) { return candidate.first.IsNearEqual(plane, PlaneTolerance); });
		if (group != planeGroups.end())
			group->second.push_back(triangle);
		else
			planeGroups.emplace_back(plane, std::vector<PlanesAndMeshes::Physics::Triangle>{ triangle });
	}

	return planeGroups;
}

void A(auto&&...args)
{
	for (auto&& arg : { std::forward<decltype(args)>(args)... })
	{
		std::cout << arg << " ";
	}
	std::cout << std::endl;
}

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	auto triangles = std::vector{ LoadFile() };
	Log::Info("Loaded {} triangles", triangles.size());

	auto planeGroups = GroupTrianglesByPlane(triangles);
	for (auto& [plane, triangles] : planeGroups)
	{
		Log::Info("Plane: Normal({}, {}, {}), D = {}, Triangles = {}", plane.Normal.x, plane.Normal.y, plane.Normal.z, plane.D, triangles.size());
	}

	auto msg = Win32::MSG{};
	while (msg.message != Win32::Messages::Quit)
	{
		if (Win32::PeekMessageW(&msg, nullptr, 0, 0, Win32::PeekMessageFlags::Remove))
		{
			Win32::TranslateMessage(&msg);
			Win32::DispatchMessageW(&msg);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds{100});
		}
	}

	return 0;
}