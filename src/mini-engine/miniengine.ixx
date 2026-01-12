export module miniengine;
import std;
import shared;



export auto WinMain(Win32::HINSTANCE hInstance, Win32::HINSTANCE hPrevInstance, Win32::PWSTR pCmdLine, int nCmdShow) -> int
try
{
	return 0;
}
catch (const std::exception& e)
{
	Log::Error("Error in main: {}", e.what());
	return -1;
}
