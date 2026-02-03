#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DXGI.lib")

import shared;

struct InitD3D12App : Shared::D3D12App
{
	InitD3D12App() { Initialise(); }
	void Initialise(this InitD3D12App& self)
	{
		self.window = UI::Window{800, 600};
		self.d3d12State = Shared::D3D12State{&self.window};
	}

	void OnIdle(this InitD3D12App& self)
	{
		// Idle processing for D3D12 initialization can go here
	}
	void F(this InitD3D12App& self) { std::println("A"); }
};

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	InitD3D12App app;
	return static_cast<int>(app.MainLoop());
}
