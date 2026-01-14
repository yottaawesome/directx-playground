#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DXGI.lib")

import shared;

struct D3D12State
{
	/* Steps
	1. Create the ID3D12Device using the D3D12CreateDevice function.
	2. Create an ID3D12Fence object and query descriptor sizes.
	3. Check 4X MSAA quality level support.
	4. Create the command queue, command list allocator, and main command list.
	5. Describe and create the swap chain.
	6. Create the descriptor heaps the application requires.
	*/
	void Initialise(this D3D12State& self)
	{
		self.InitDxInfrastructure()
			.InitDescriptorSizes()
			.CheckMsaaQualityLevels()
			.CreateCommandObjects()
			.CreateSwapChain()
			.CreateDescriptorHeaps();
	}

	auto CheckMsaaQualityLevels(this D3D12State& self) -> decltype(auto)
	{
		D3D12::D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels{
			.Format = self.backBufferFormat,
			.SampleCount = 4,
			.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
			.NumQualityLevels = 0
		};
		Com::HResult hr = self.d3d12Device->CheckFeatureSupport(
			D3D12::D3D12_FEATURE::D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&qualityLevels,
			sizeof(qualityLevels)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to check 4X MSAA quality level support");
		self.msaaQualityLevels = qualityLevels.NumQualityLevels;
		return self;
	}

	auto InitDxInfrastructure(this D3D12State& self) -> decltype(auto)
	{
		Com::HResult hr = DXGI::CreateDXGIFactory1(
			self.dxgiFactory.GetUuid(),
			std::out_ptr(self.dxgiFactory)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create DXGI Factory");

		hr = D3D12::D3D12CreateDevice(
			nullptr, // use default adapter
			D3D12::D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_2, // https://docs.microsoft.com/en-us/windows/desktop/api/d3dcommon/ne-d3dcommon-d3d_feature_level
			self.d3d12Device.GetUuid(),
			std::out_ptr(self.d3d12Device) //Win32::IID_PPV_ARGS_Helper(&d3d12Device)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Device");

		hr = self.d3d12Device->CreateFence(
			0,
			D3D12::D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE,
			self.fence.GetUuid(),
			std::out_ptr(self.fence)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Fence");
		return self;
	}

	auto InitDescriptorSizes(this D3D12State& self) -> decltype(auto)
	{
		self.rtvDescriptorSize = self.d3d12Device->GetDescriptorHandleIncrementSize(D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		self.dsvDescriptorSize = self.d3d12Device->GetDescriptorHandleIncrementSize(D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		self.cbvSrvUavDescriptorSize = self.d3d12Device->GetDescriptorHandleIncrementSize(D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		return self;
	}

	// TODO: Implement command queue, command list allocator, and command list creation
	auto CreateCommandObjects(this D3D12State& self) -> decltype(auto)
	{
		D3D12::D3D12_COMMAND_QUEUE_DESC queueDesc{
			.Type = D3D12::D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Flags = D3D12::D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE,
		};
		Com::HResult hr = self.d3d12Device->CreateCommandQueue(
			&queueDesc,
			self.commandQueue.GetUuid(),
			std::out_ptr(self.commandQueue)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Command Queue");

		hr = self.d3d12Device->CreateCommandAllocator(
			D3D12::D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			self.commandAllocator.GetUuid(),
			std::out_ptr(self.commandAllocator)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Command Allocator");

		hr = self.d3d12Device->CreateCommandList(
			0,
			D3D12::D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			self.commandAllocator.get(),
			nullptr,
			self.commandList.GetUuid(),
			std::out_ptr(self.commandList)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Command List");
		// Needs to be Close()d before we can call Reset() on it later.
		self.commandList->Close();

		return self;
	}

	// TODO: Implement swap chain creation
	auto CreateSwapChain(this D3D12State& self) -> decltype(auto)
	{
		return self;
	}

	// TODO: Implement descriptor heap creation
	auto CreateDescriptorHeaps(this D3D12State& self) -> decltype(auto)
	{
		return self;
	}

	DXGI::DXGI_FORMAT backBufferFormat = DXGI::DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

	Com::Ptr<D3D12::ID3D12Device> d3d12Device;
	Com::Ptr<DXGI::IDXGIFactory4> dxgiFactory;
	Com::Ptr<D3D12::ID3D12Fence> fence;

	Com::Ptr<D3D12::ID3D12CommandQueue> commandQueue;
	Com::Ptr<D3D12::ID3D12CommandAllocator> commandAllocator;
	Com::Ptr<D3D12::ID3D12GraphicsCommandList> commandList;
	Com::Ptr<D3D12::ID3D12DescriptorHeap> descriptorHeap;

	// render target view descriptor size
	std::uint32_t rtvDescriptorSize = 0;
	// depth stencil view descriptor size
	std::uint32_t dsvDescriptorSize = 0;
	// constant buffer view / shader resource view / unordered access view descriptor size
	std::uint32_t cbvSrvUavDescriptorSize = 0;
	std::uint32_t msaaQualityLevels = 0;
};

struct InitD3D12App : Shared::D3D12App
{
	void Initialise(this InitD3D12App& self)
	{
		self.window.Initialise();
		self.d3d12State.Initialise();
	}

	void OnIdle(this InitD3D12App& self)
	{
		// Idle processing for D3D12 initialization can go here
	}

	UI::Window window;
	D3D12State d3d12State;
};

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	InitD3D12App app;
	app.Initialise();
	return static_cast<int>(app.MainLoop());
}
