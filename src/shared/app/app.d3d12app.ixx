export module shared:app.d3d12app;
import std;
import :app.windowedapp;
import :win32;
import :com;
import :error;
import :async;

export namespace App
{
	class D3D12App : public App::WindowedApp
	{
	public:
		virtual ~D3D12App()
		{
			FlushCommandQueue();
		}

		D3D12App(std::uint32_t width, std::uint32_t height)
			: WindowedApp(width, height)
		{ }

		auto MainLoop(this auto&& self) -> Win32::LRESULT
		{
			auto msg = Win32::MSG{};
			while (msg.message != Win32::Messages::Quit)
			{
				if (Win32::PeekMessageW(&msg, nullptr, 0, 0, Win32::PeekMessageOptions::Remove))
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

		void OnIdle(this auto& self)
		{
			// Default idle processing does nothing.
		}

		auto AsD3D12App(this D3D12App& self) -> D3D12App&
		{
			return self;
		}

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
		void InitialiseD3D12(this auto& self)
		{
			auto [width, height] = self.GetDimensions();
			self.InitDxInfrastructure()
				.InitDescriptorSizes()
				.CheckMsaaQualityLevels()
				.CreateCommandObjects()
				.CreateSwapChain(width, height)
				.CreateDescriptorHeaps()
				.CreateRenderTargetViews()
				.CreateDepthStencilView(width, height)
				.SetViewportAndScissorRect(width, height);
		}

		auto CheckMsaaQualityLevels(this auto& self) -> decltype(auto)
		{
			auto qualityLevels = D3D12::D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS{
				.Format = self.backBufferFormat,
				.SampleCount = 4,
				.Flags = D3D12::D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS::D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
				.NumQualityLevels = 0
			};
			auto hr = Com::HResult{
				self.d3d12Device->CheckFeatureSupport(
					D3D12::D3D12_FEATURE::D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
					&qualityLevels,
					sizeof(qualityLevels)
				) };
			if (not hr)
				throw Error::ComError(hr, "Failed to check 4X MSAA quality level support");
			self.msaaQualityLevels = qualityLevels.NumQualityLevels;
			return self;
		}

		auto InitDxInfrastructure(this auto& self) -> decltype(auto)
		{
			auto hr = Com::HResult{
				DXGI::CreateDXGIFactory1(self.dxgiFactory.GetUuid(), std::out_ptr(self.dxgiFactory))
			};
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

		void FlushCommandQueue(this auto& self)
		{
			// Advance the fence value to mark commands up to this fence point.
			self.currentFence++;
			auto hr = Com::HResult{ self.commandQueue->Signal(self.fence.get(), self.currentFence) };
			if (not hr)
				throw Error::ComError(hr, "Failed to signal command queue");
			// Wait until the GPU has completed commands up to this fence point.
			if (self.fence->GetCompletedValue() < self.currentFence)
			{
				Async::Event eventHandle{};
				hr = self.fence->SetEventOnCompletion(self.currentFence, eventHandle.GetHandle());
				if (not hr)
					throw Error::ComError(hr, "Failed to set fence event");
				eventHandle.Wait();
			}
		}

		auto InitDescriptorSizes(this auto& self) -> decltype(auto)
		{
			self.rtvDescriptorSize = self.d3d12Device->GetDescriptorHandleIncrementSize(D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			self.dsvDescriptorSize = self.d3d12Device->GetDescriptorHandleIncrementSize(D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			self.cbvSrvUavDescriptorSize = self.d3d12Device->GetDescriptorHandleIncrementSize(D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			return self;
		}

		auto CreateCommandObjects(this auto& self) -> decltype(auto)
		{
			auto queueDesc = D3D12::D3D12_COMMAND_QUEUE_DESC{
				.Type = D3D12::D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
				.Flags = D3D12::D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE,
			};
			auto hr = Com::HResult{
				self.d3d12Device->CreateCommandQueue(
					&queueDesc,
					self.commandQueue.GetUuid(),
					std::out_ptr(self.commandQueue)
				) };
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

		auto CreateSwapChain(this auto& self, std::uint32_t width, std::uint32_t height) -> decltype(auto)
		{
			self.swapChain.reset();
			auto swapChainDesc = DXGI::DXGI_SWAP_CHAIN_DESC{
				.BufferDesc{
					.Width = width,
					.Height = height,
					.RefreshRate{
						.Numerator = 60,
						.Denominator = 1,
					},
					.Format = self.backBufferFormat,
					.ScanlineOrdering = DXGI::DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
					.Scaling = DXGI::DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED
				},
				.SampleDesc{
					.Count = self.msaa4xState ? 4u : 1u,
					.Quality = self.msaa4xState ? (self.msaaQualityLevels - 1) : 0,
				},
				.BufferUsage = DXGI::UsageRenderTargetOutput,
				.BufferCount = self.swapChainBufferCount,
				.OutputWindow = self.GetHandle(),
				.Windowed = true,
				.SwapEffect = DXGI::DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.Flags = DXGI::DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
			};
			auto hr = Com::HResult{
				self.dxgiFactory->CreateSwapChain(
					self.commandQueue.get(),
					&swapChainDesc,
					std::out_ptr(self.swapChain)
				) };
			if (not hr)
				throw Error::ComError(hr, "Failed to create DXGI Swap Chain");

			return self;
		}

		auto CreateDescriptorHeaps(this auto& self) -> decltype(auto)
		{
			auto rtvHeapDesc = D3D12::D3D12_DESCRIPTOR_HEAP_DESC{
				.Type = D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				.NumDescriptors = self.swapChainBufferCount,
				.Flags = D3D12::D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
				.NodeMask = 0
			};
			auto hr = Com::HResult{
				self.d3d12Device->CreateDescriptorHeap(
					&rtvHeapDesc,
					self.descriptorHeap.GetUuid(),
					std::out_ptr(self.descriptorHeap)
				) };
			if (not hr)
				throw Error::ComError(hr, "Failed to create RTV Descriptor Heap");

			auto dsvHeapDesc = D3D12::D3D12_DESCRIPTOR_HEAP_DESC{
				.Type = D3D12::D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
				.NumDescriptors = 1,
				.Flags = D3D12::D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
				.NodeMask = 0
			};
			hr = self.d3d12Device->CreateDescriptorHeap(&dsvHeapDesc, self.dsvHeap.GetUuid(), std::out_ptr(self.dsvHeap));
			if (not hr)
				throw Error::ComError(hr, "Failed to create DSV Descriptor Heap");

			return self;
		}

		auto CreateRenderTargetViews(this auto& self) -> decltype(auto)
		{
			auto rtvHandle = D3D12::D3D12_CPU_DESCRIPTOR_HANDLE{ self.descriptorHeap->GetCPUDescriptorHandleForHeapStart() };

			self.renderTargets.resize(self.swapChainBufferCount);
			for (std::uint32_t i = 0; i < self.swapChainBufferCount; ++i)
			{
				auto hr = Com::HResult{
					self.swapChain->GetBuffer(i, self.renderTargets[i].GetUuid(), std::out_ptr(self.renderTargets[i]))
				};
				if (not hr)
					throw Error::ComError(hr, "Failed to get Swap Chain Buffer");

				self.d3d12Device->CreateRenderTargetView(
					self.renderTargets[i].get(),
					nullptr,
					rtvHandle
				);
				rtvHandle.ptr += self.rtvDescriptorSize;
			}
			return self;
		}

		auto CreateDepthStencilView(this auto& self, std::uint32_t width, std::uint32_t height) -> decltype(auto)
		{
			auto depthStencilDesc = D3D12::D3D12_RESOURCE_DESC{
				.Dimension = D3D12::D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				.Alignment = 0,
				.Width = width,
				.Height = height,
				.DepthOrArraySize = 1,
				.MipLevels = 1,
				.Format = DXGI::DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,
				.SampleDesc{
					.Count = self.msaa4xState ? 4u : 1u,
					.Quality = self.msaa4xState ? (self.msaaQualityLevels - 1) : 0,
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
			auto heapProps = D3D12::CD3DX12_HEAP_PROPERTIES(D3D12::D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT);
			auto hr = Com::HResult{
				self.d3d12Device->CreateCommittedResource(
					&heapProps,
					D3D12::D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
					&depthStencilDesc,
					D3D12::D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
					&clearValue,
					self.depthStencilBuffer.GetUuid(),
					std::out_ptr(self.depthStencilBuffer)
				) };
			if (not hr)
				throw Error::ComError(hr, "Failed to create Depth Stencil Buffer");
			self.d3d12Device->CreateDepthStencilView(self.depthStencilBuffer.get(), nullptr, self.dsvHeap->GetCPUDescriptorHandleForHeapStart());
			return self;
		}

		auto SetViewportAndScissorRect(this auto& self, std::uint32_t width, std::uint32_t height) -> decltype(auto)
		{
			self.viewport = D3D12::D3D12_VIEWPORT{
				.TopLeftX = 0.0f,
				.TopLeftY = 0.0f,
				.Width = static_cast<float>(width),
				.Height = static_cast<float>(height),
				.MinDepth = 0.0f,
				.MaxDepth = 1.0f
			};
			self.scissorRect = D3D12::D3D12_RECT{
				.left = 0,
				.top = 0,
				.right = static_cast<Win32::LONG>(width),
				.bottom = static_cast<Win32::LONG>(height)
			};
			return self;
		}

#pragma region Fields
		bool msaa4xState = false;
		DXGI::DXGI_FORMAT backBufferFormat = DXGI::DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

		Com::Ptr<D3D12::ID3D12Device> d3d12Device;
		Com::Ptr<DXGI::IDXGIFactory4> dxgiFactory;
		std::uint64_t currentFence = 0;
		Com::Ptr<D3D12::ID3D12Fence> fence;

		// The GPU has a command queue, and commands are submitted to the queue via one or more command lists.
		// Command lists are open or closed; they must be open for submitting commands, but must be closed 
		// before they are submitted and can be executed by the command queue. Commands are executed asynchronously.
		Com::Ptr<D3D12::ID3D12GraphicsCommandList> commandList;
		Com::Ptr<D3D12::ID3D12CommandQueue> commandQueue;
		// Associated with command lists are command allocators, which manage the underlying memory that the 
		// command lists record their commands into. Command allocators can have multiple command lists 
		// associated with them, so long as only one command list is recording at a time. Allocators can be 
		// reset, but only when the GPU is no longer using the memory associated with the allocator.
		Com::Ptr<D3D12::ID3D12CommandAllocator> commandAllocator;
		// Note that "descriptor" and "view" are synonymous in D3D12.
		Com::Ptr<D3D12::ID3D12DescriptorHeap> descriptorHeap;
		Com::Ptr<D3D12::ID3D12DescriptorHeap> dsvHeap;
		// A swap chain is the front and back buffer collection that is used for rendering and presenting frames to 
		// the display.
		Com::Ptr<DXGI::IDXGISwapChain> swapChain;
		Com::Ptr<D3D12::ID3D12Resource> depthStencilBuffer{};
		constexpr static std::uint32_t swapChainBufferCount = 2;
		Com::Ptr<D3D12::ID3D12Resource> swapChainBuffer[swapChainBufferCount];
		std::vector<Com::Ptr<D3D12::ID3D12Resource>> renderTargets;

		// RTV = Render Target View
		// DSV = Depth Stencil View
		// render target view descriptor size
		std::uint32_t rtvDescriptorSize = 0;
		// depth stencil view descriptor size
		std::uint32_t dsvDescriptorSize = 0;
		// constant buffer view / shader resource view / unordered access view descriptor size
		std::uint32_t cbvSrvUavDescriptorSize = 0;
		std::uint32_t msaaQualityLevels = 0;

		D3D12::D3D12_VIEWPORT viewport{};
		D3D12::D3D12_RECT scissorRect{};
#pragma endregion
	};
}
