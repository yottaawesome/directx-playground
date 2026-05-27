#pragma comment(lib, "d3d11.lib")

import std;
import dx3d;

auto main() -> int
try
{
	auto game = dx3d::Game{};
	game.Run();
	return 0;
}
catch (const std::runtime_error& ex)
{
	std::cerr << "Runtime error: " << ex.what() << std::endl;
	return 1;
}
catch (const std::exception& ex)
{
	std::cerr << "Error: " << ex.what() << std::endl;
	return 1;
}
catch (...)
{
	std::cerr << "An unknown error occurred." << std::endl;
	return 1;
}
