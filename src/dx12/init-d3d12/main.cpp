#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DXGI.lib")

import shared;

struct InitD3D12App : App::D3D12App
{
	InitD3D12App() : D3D12App(800, 600) 
	{ 
		InitialiseWindow(); 
		InitialiseD3D12();
	}

	void OnIdle(this InitD3D12App& self)
	{
		self.AsD3D12App().OnIdle();
	}

	auto OnMessage(
		this InitD3D12App& self,
		const App::Win32Message<Win32::Messages::Size>& message
	) -> Win32::LRESULT
	{
		return 0;
	}
};

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	InitD3D12App app;
	return static_cast<int>(app.MainLoop());
}
