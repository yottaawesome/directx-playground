import std;
import Shared;
import PlanesAndMeshes;

[[nodiscard]]
auto TokeniseString(const std::string& stringToTokenise, const std::string& delimiter) -> std::vector<std::string>
{
	auto position = size_t{};
	// If we don't find it at all, add the whole string
	if (stringToTokenise.find(delimiter, position) == std::string::npos)
		return { stringToTokenise };

	auto results = std::vector<std::string>{};
	auto intermediateString = std::string{ stringToTokenise };
	while ((position = intermediateString.find(delimiter)) != std::string::npos)
	{
		// split and add to the results
		auto split = std::string{ intermediateString.substr(0, position) };
		results.push_back(split);

		// move up our position
		position += delimiter.length();
		intermediateString = intermediateString.substr(position);

		// On the last iteration, enter the remainder
		if (intermediateString.find(delimiter) == std::string::npos)
			results.push_back(intermediateString);
	}

	return results;
}

[[nodiscard]]
auto LoadFile() -> std::vector<Physics::Triangle>
{
	constexpr auto FilePath = "mesh.txt";

	auto inputFile = std::ifstream{ FilePath };
	if (inputFile.fail())
		throw std::runtime_error{ std::format("Failed to open {}", FilePath) };

	auto float3s = std::vector<DirectX::XMFLOAT3>{};

	auto line = std::string{};
	while (not inputFile.eof())
	{
		std::getline(inputFile, line);
		if (line.empty())
			continue;
		if (line.starts_with("Vertices:"))
			continue;
		// Don't do anything with indices for now
		if (line.starts_with("Indices:"))
			break;

		auto parts = TokeniseString(line, " ");
		if (parts.size() != 3)
			throw std::runtime_error{ std::format("Expected 3 values, got {}", parts.size()) };

		float3s.emplace_back(std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]));
	}

	auto triples = float3s | std::ranges::views::chunk(3);
	auto returnValue = std::vector<Physics::Triangle>{};
	for (auto chunk : triples)
	{
		returnValue.push_back(Physics::Triangle{chunk[0], chunk[1], chunk[2]});
	}

	return returnValue;
}

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	auto triangles = std::vector{ LoadFile() };
	Log::Info("Loaded {} triangles", triangles.size());

	auto planeToTrianglesMap = std::unordered_map<Physics::Plane, std::vector<Physics::Triangle>, Physics::PlaneHash>{};
	for (const Physics::Triangle& triangle : triangles)
	{
		auto plane = triangle.GetPlane();
		if (planeToTrianglesMap.contains(plane))
		{
			planeToTrianglesMap[plane].push_back(triangle);
		}
		else
		{
			planeToTrianglesMap[plane] = std::vector<Physics::Triangle>{ triangle };
		}
	}

	for (auto& [plane, triangles] : planeToTrianglesMap)
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