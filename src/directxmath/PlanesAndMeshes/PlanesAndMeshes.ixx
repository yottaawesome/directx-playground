export module PlanesAndMeshes;
import std;
import Shared;

export namespace PlanesAndMeshes
{
	[[nodiscard]]
	auto LoadFile() -> std::vector<Math::Triangle>
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
		auto returnValue = std::vector<Math::Triangle>{};
		returnValue.reserve(indices.size() / 3);
		for (auto chunk : triples)
		{
			returnValue.push_back(Math::Triangle{ float3s[chunk[0]], float3s[chunk[1]], float3s[chunk[2]] });
		}

		return returnValue;
	}

	using PlaneGroup = std::pair<Math::Plane, std::vector<Math::Triangle>>;

	[[nodiscard]]
	auto GroupTrianglesByPlane(const std::vector<Math::Triangle>& triangles) -> std::vector<PlaneGroup>
	{
		constexpr auto PlaneTolerance = 1e-5f;
		auto planeGroups = std::vector<PlaneGroup>{};
		for (const Math::Triangle& triangle : triangles)
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
				planeGroups.emplace_back(plane, std::vector<Math::Triangle>{ triangle });
		}

		return planeGroups;
	}
}