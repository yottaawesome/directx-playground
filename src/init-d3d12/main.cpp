#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DXGI.lib")

import shared;

struct InitD3D12App
{
	UI::Window MainWindow;

	InitD3D12App() { Initialise(); }

	void OnIdle(this InitD3D12App& self)
	{
		// Idle processing for D3D12 initialization can go here
	}

	auto MainLoop(this auto&& self) -> Win32::LRESULT
	{
		Win32::MSG msg{};
		while (msg.message != Win32::Messages::Quit)
		{
			if (Win32::PeekMessageW(&msg, self.MainWindow.GetHandle(), 0, 0, Win32::PeekMessageOptions::Remove))
			{
				Win32::TranslateMessage(&msg);
				Win32::DispatchMessageW(&msg);
			}
			else
			{
				self.OnIdle();
			}
		}
		return msg.wParam;
	}

	// D3D12 functions
	/* Steps
		1. Create the ID3D12Device using the D3D12CreateDevice function.
		2. Create an ID3D12Fence object and query descriptor sizes.
		3. Check 4X MSAA quality level support.
		4. Create the command queue, command list allocator, and main command list.
		5. Describe and create the swap chain.
		6. Create the descriptor heaps the application requires.
		7. Create the render target views (RTVs) for the swap chain back buffers.
		8. Create a depth/stencil buffer and its descriptor heap and view.
		9. Set up the viewport and scissor rectangle.
	*/
	void Initialise(this InitD3D12App& self)
	{
		self.MainWindow = UI::Window{ 800, 600 };
		self.InitDxInfrastructure()
			.InitDescriptorSizes()
			.CheckMsaaQualityLevels()
			.CreateCommandObjects()
			.CreateSwapChain()
			.CreateDescriptorHeaps()
			.CreateRenderTargetViews()
			.CreateDepthStencilView()
			.SetViewportAndScissorRect();
	}

	auto CheckMsaaQualityLevels(this InitD3D12App& self) -> decltype(auto)
	{
		auto qualityLevels = D3D12::D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS{
			.Format = self.BackBufferFormat,
			.SampleCount = 4,
			.Flags = D3D12::D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS::D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
			.NumQualityLevels = 0
		};
		auto hr = Com::HResult{
			self.D3d12Device->CheckFeatureSupport(
				D3D12::D3D12_FEATURE::D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
				&qualityLevels,
				sizeof(qualityLevels)
			) };
		if (not hr)
			throw Error::ComError(hr, "Failed to check 4X MSAA quality level support");
		self.MsaaQualityLevels = qualityLevels.NumQualityLevels;
		return self;
	}

	auto InitDxInfrastructure(this InitD3D12App& self) -> decltype(auto)
	{
		auto hr = Com::HResult{
			DXGI::CreateDXGIFactory1(self.DxgiFactory.GetUuid(), std::out_ptr(self.DxgiFactory))
		};
		if (not hr)
			throw Error::ComError(hr, "Failed to create DXGI Factory");

		hr = D3D12::D3D12CreateDevice(
			nullptr, // use default adapter
			D3D12::D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_2, // https://docs.microsoft.com/en-us/windows/desktop/api/d3dcommon/ne-d3dcommon-d3d_feature_level
			self.D3d12Device.GetUuid(),
			std::out_ptr(self.D3d12Device) //Win32::IID_PPV_ARGS_Helper(&d3d12Device)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Device");

		hr = self.D3d12Device->CreateFence(
			0,
			D3D12::D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE,
			self.Fence.GetUuid(),
			std::out_ptr(self.Fence)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Fence");
		return self;
	}

	void FlushCommandQueue(this InitD3D12App& self)
	{
		// Advance the fence value to mark commands up to this fence point.
		self.currentFence++;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		self.CommandQueue->Signal(self.Fence.Get(), self.currentFence);

		// Wait until the GPU has completed commands up to this fence point.
		if (self.Fence->GetCompletedValue() < self.currentFence)
		{
			Win32::HANDLE eventHandle = Win32::CreateEventExW(nullptr, nullptr, 0, Win32::EventAccess::All);

			// Fire event when GPU hits current fence.  
			self.Fence->SetEventOnCompletion(self.currentFence, eventHandle);

			// Wait until the GPU hits current fence event is fired.
			Win32::WaitForSingleObject(eventHandle, Win32::Infinite);
			Win32::CloseHandle(eventHandle);
		}
	}

	auto InitDescriptorSizes(this InitD3D12App& self) -> decltype(auto)
	{
		self.RtvDescriptorSize = self.D3d12Device->GetDescriptorHandleIncrementSize(D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		self.DsvDescriptorSize = self.D3d12Device->GetDescriptorHandleIncrementSize(D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		self.CbvSrvUavDescriptorSize = self.D3d12Device->GetDescriptorHandleIncrementSize(D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		return self;
	}

	auto CreateCommandObjects(this InitD3D12App& self) -> decltype(auto)
	{
		auto queueDesc = D3D12::D3D12_COMMAND_QUEUE_DESC{
			.Type = D3D12::D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Flags = D3D12::D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE,
		};
		auto hr = Com::HResult{
			self.D3d12Device->CreateCommandQueue(
				&queueDesc,
				self.CommandQueue.GetUuid(),
				std::out_ptr(self.CommandQueue)
			) };
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Command Queue");

		hr = self.D3d12Device->CreateCommandAllocator(
			D3D12::D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			self.CommandAllocator.GetUuid(),
			std::out_ptr(self.CommandAllocator)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Command Allocator");

		hr = self.D3d12Device->CreateCommandList(
			0,
			D3D12::D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			self.CommandAllocator.get(),
			nullptr,
			self.CommandList.GetUuid(),
			std::out_ptr(self.CommandList)
		);
		if (not hr)
			throw Error::ComError(hr, "Failed to create D3D12 Command List");
		// Needs to be Close()d before we can call Reset() on it later.
		self.CommandList->Close();

		return self;
	}

	auto CreateSwapChain(this InitD3D12App& self) -> decltype(auto)
	{
		self.SwapChain.reset();
		auto swapChainDesc = DXGI::DXGI_SWAP_CHAIN_DESC{
			.BufferDesc{
				.Width = self.MainWindow.GetDimensions().Width,
				.Height = self.MainWindow.GetDimensions().Height,
				.RefreshRate{
					.Numerator = 60,
					.Denominator = 1,
				},
				.Format = self.BackBufferFormat,
				.ScanlineOrdering = DXGI::DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
				.Scaling = DXGI::DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED
			},
			.SampleDesc{
				.Count = self.Msaa4xState ? 4u : 1u,
				.Quality = self.Msaa4xState ? (self.MsaaQualityLevels - 1) : 0,
			},
			.BufferUsage = DXGI::UsageRenderTargetOutput,
			.BufferCount = self.SwapChainBufferCount,
			.OutputWindow = self.MainWindow.GetHandle(),
			.Windowed = true,
			.SwapEffect = DXGI::DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = DXGI::DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		};
		auto hr = Com::HResult{
			self.DxgiFactory->CreateSwapChain(
				self.CommandQueue.get(),
				&swapChainDesc,
				std::out_ptr(self.SwapChain)
			) };
		if (not hr)
			throw Error::ComError(hr, "Failed to create DXGI Swap Chain");

		return self;
	}

	auto CreateDescriptorHeaps(this InitD3D12App& self) -> decltype(auto)
	{
		auto rtvHeapDesc = D3D12::D3D12_DESCRIPTOR_HEAP_DESC{
			.Type = D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = self.SwapChainBufferCount,
			.Flags = D3D12::D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0
		};
		auto hr = Com::HResult{
			self.D3d12Device->CreateDescriptorHeap(
				&rtvHeapDesc,
				self.DescriptorHeap.GetUuid(),
				std::out_ptr(self.DescriptorHeap)
			) };
		if (not hr)
			throw Error::ComError(hr, "Failed to create RTV Descriptor Heap");

		auto dsvHeapDesc = D3D12::D3D12_DESCRIPTOR_HEAP_DESC{
			.Type = D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			.NumDescriptors = 1,
			.Flags = D3D12::D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0
		};
		hr = self.D3d12Device->CreateDescriptorHeap(&dsvHeapDesc, self.DsvHeap.GetUuid(), std::out_ptr(self.DsvHeap));
		if (not hr)
			throw Error::ComError(hr, "Failed to create DSV Descriptor Heap");

		return self;
	}

	auto CreateRenderTargetViews(this InitD3D12App& self) -> decltype(auto)
	{
		auto rtvHandle = D3D12::D3D12_CPU_DESCRIPTOR_HANDLE{ self.DescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		for (std::uint32_t i = 0; i < self.SwapChainBufferCount; ++i)
		{
			auto backBuffer = Com::Ptr<D3D12::ID3D12Resource>{};
			auto hr = Com::HResult{
				self.SwapChain->GetBuffer(i, backBuffer.GetUuid(), std::out_ptr(backBuffer))
			};
			if (not hr)
				throw Error::ComError(hr, "Failed to get Swap Chain Buffer");
			self.D3d12Device->CreateRenderTargetView(
				backBuffer.get(),
				nullptr,
				rtvHandle
			);
			rtvHandle.ptr += self.RtvDescriptorSize;
		}
		return self;
	}

	auto CreateDepthStencilView(this InitD3D12App& self) -> decltype(auto)
	{
		auto depthStencilDesc = D3D12::D3D12_RESOURCE_DESC{
			.Dimension = D3D12::D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Alignment = 0,
			.Width = self.MainWindow.GetDimensions().Width,
			.Height = self.MainWindow.GetDimensions().Height,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI::DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,
			.SampleDesc{
				.Count = self.Msaa4xState ? 4u : 1u,
				.Quality = self.Msaa4xState ? (self.MsaaQualityLevels - 1) : 0,
			},
			.Layout = D3D12::D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12::D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		};
		auto clearValue = D3D12::D3D12_CLEAR_VALUE{
			.Format = DXGI::DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,
			.DepthStencil{
				.Depth = 1.0f,
				.Stencil = 0
			}
		};
		auto depthStencilBuffer = Com::Ptr<D3D12::ID3D12Resource>{};
		auto heapProps = D3D12::CD3DX12_HEAP_PROPERTIES(D3D12::D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT);
		auto hr = Com::HResult{
			self.D3d12Device->CreateCommittedResource(
				&heapProps,
				D3D12::D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
				&depthStencilDesc,
				D3D12::D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
				&clearValue,
				depthStencilBuffer.GetUuid(),
				std::out_ptr(depthStencilBuffer)
			) };
		if (not hr)
			throw Error::ComError(hr, "Failed to create Depth Stencil Buffer");
		self.D3d12Device->CreateDepthStencilView(depthStencilBuffer.get(), nullptr, self.DsvHeap->GetCPUDescriptorHandleForHeapStart());
		return self;
	}

	auto SetViewportAndScissorRect(this InitD3D12App& self) -> decltype(auto)
	{
		auto viewport = D3D12::D3D12_VIEWPORT{
			.TopLeftX = 0.0f,
			.TopLeftY = 0.0f,
			.Width = static_cast<float>(self.MainWindow.GetWidth()),
			.Height = static_cast<float>(self.MainWindow.GetHeight()),
			.MinDepth = 0.0f,
			.MaxDepth = 1.0f
		};
		auto scissorRect = D3D12::D3D12_RECT{
			.left = 0,
			.top = 0,
			.right = static_cast<LONG>(self.MainWindow.GetWidth()),
			.bottom = static_cast<LONG>(self.MainWindow.GetHeight())
		};
		return self;
	}

#pragma region Fields
	bool Msaa4xState = false;
	DXGI::DXGI_FORMAT BackBufferFormat = DXGI::DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

	Com::Ptr<D3D12::ID3D12Device> D3d12Device;
	Com::Ptr<DXGI::IDXGIFactory4> DxgiFactory;
	std::uint64_t currentFence = 0;
	Com::Ptr<D3D12::ID3D12Fence> Fence;

	// The GPU has a command queue, and commands are submitted to the queue via one or more command lists.
	// Command lists are open or closed; they must be open for submitting commands, but must be closed 
	// before they are submitted and can be executed by the command queue. Commands are executed asynchronously.
	Com::Ptr<D3D12::ID3D12GraphicsCommandList> CommandList;
	Com::Ptr<D3D12::ID3D12CommandQueue> CommandQueue;
	// Associated with command lists are command allocators, which manage the underlying memory that the 
	// command lists record their commands into. Command allocators can have multiple command lists 
	// associated with them, so long as only one command list is recording at a time. Allocators can be 
	// reset, but only when the GPU is no longer using the memory associated with the allocator.
	Com::Ptr<D3D12::ID3D12CommandAllocator> CommandAllocator;
	// Note that "descriptor" and "view" are synonymous in D3D12.
	Com::Ptr<D3D12::ID3D12DescriptorHeap> DescriptorHeap;
	Com::Ptr<D3D12::ID3D12DescriptorHeap> DsvHeap;
	// A swap chain is the front and back buffer collection that is used for rendering and presenting frames to 
	// the display.
	Com::Ptr<DXGI::IDXGISwapChain> SwapChain;

	// RTV = Render Target View
	// DSV = Depth Stencil View
	// render target view descriptor size
	std::uint32_t RtvDescriptorSize = 0;
	// depth stencil view descriptor size
	std::uint32_t DsvDescriptorSize = 0;
	// constant buffer view / shader resource view / unordered access view descriptor size
	std::uint32_t CbvSrvUavDescriptorSize = 0;
	std::uint32_t MsaaQualityLevels = 0;
	std::uint32_t SwapChainBufferCount = 2;
#pragma endregion
};

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	InitD3D12App app;
	return static_cast<int>(app.MainLoop());
}
