export module dx3d:graphics.graphicsdevice;
import std;
import :core;
import :win32;
import :com;
import :graphics.swapchain;
import :graphics.graphicsresource;
import :graphics.devicecontext;

namespace dx3d
{
	struct GraphicsDeviceDesc
	{
		BaseDesc Base;
	};
	class GraphicsDevice final : public Base
	{
	public:
		virtual ~GraphicsDevice() override = default;
		GraphicsDevice(const GraphicsDeviceDesc& desc)
			: Base(desc.Base)
		{
			constexpr auto createDeviceFlags = 
				[] consteval -> unsigned
				{
					auto createDeviceFlags = 0u;
					if constexpr (IsDebug)
						createDeviceFlags |= D3D11::D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
					return createDeviceFlags;
				}();

			constexpr auto featureLevels = std::array{
				D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
			};
			auto createdFeatureLevel = D3D11::D3D_FEATURE_LEVEL{};
			auto hr = HResult{ 
				D3D11::D3D11CreateDevice(
					nullptr,
					D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
					nullptr,
					createDeviceFlags,
					featureLevels.data(),
					static_cast<Win32::UINT>(featureLevels.size()),
					D3D11::SdkVersion,
					device.ReleaseAndGetAddressOf(),
					&createdFeatureLevel,
					d3dDeviceContext.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw ComError{ hr, "Failed to create D3D11 device and context." };

			dxgiDevice = device.As<DXGI::IDXGIDevice>();
			
			if (hr = dxgiDevice->GetParent(dxgiAdapter.GetUuid(), dxgiAdapter.AddressOf()); not hr)
				throw ComError{ hr, "Failed to get DXGI adapter from D3D11 device." };
			if (hr = dxgiAdapter->GetParent(dxgiFactory.GetUuid(), dxgiFactory.AddressOf()); not hr)
				throw ComError{ hr, "Failed to get DXGI factory from DXGI adapter." };
		}

		auto CreateSwapChain(const SwapChainDesc& desc) const
			-> std::shared_ptr<SwapChain>
		{
			return std::make_shared<SwapChain>(
				desc, 
				GraphicsResourceDesc{ GetGraphicsResourceDesc() }
			);
		}

		auto CreateDeviceContext() const
			-> std::shared_ptr<DeviceContext>
		{
			return std::make_shared<DeviceContext>(GetGraphicsResourceDesc());
		}

		void ExecuteCommandLists(DeviceContext& deferredContexts) const
		{
			auto commandList = Ptr<D3D11::ID3D11CommandList>{};
			auto hr = deferredContexts.deferredDeviceContext->FinishCommandList(false, commandList.ReleaseAndGetAddressOf());
			if(Win32::Failed(hr))
				throw ComError{ hr, "Failed to finish command list." };
			d3dDeviceContext->ExecuteCommandList(commandList.Get(), false);
		}
	private:
		auto GetGraphicsResourceDesc() const -> GraphicsResourceDesc
		{
			return GraphicsResourceDesc{ 
				logger, 
				*device.Get(),
				*dxgiFactory.Get()
			};
		}
	private:
		Ptr<D3D11::ID3D11Device> device;
		Ptr<D3D11::ID3D11DeviceContext> d3dDeviceContext; // m_d3dContext
		Ptr<DXGI::IDXGIDevice> dxgiDevice;
		Ptr<DXGI::IDXGIAdapter> dxgiAdapter;
		Ptr<DXGI::IDXGIFactory> dxgiFactory;
	};
}