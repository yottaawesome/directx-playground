import std;
import Shared;
import PlanesAndMeshes;

using namespace PlanesAndMeshes;

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