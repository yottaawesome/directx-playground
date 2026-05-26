export module miniengine;
import std;
import shared;

export extern "C" auto wWinMain(Win32::HINSTANCE instance, Win32::HINSTANCE previous, Win32::PWSTR cmd, int cmdShow) -> int
try
{
	return 0;
}
catch (const std::exception& e)
{
	Log::Error("Error in main: {}", e.what());
	return -1;
}
