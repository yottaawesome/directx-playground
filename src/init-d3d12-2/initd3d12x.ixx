#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DXGI.lib")

export module initd3d12x;
export import :approach1;
export import :approach2;
export import :common;
import std;
import shared;

export extern "C++" auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	Approach1::D3d12xApp app;
	return static_cast<int>(app.MainLoop());
}
