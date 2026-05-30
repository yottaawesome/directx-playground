export module dx3d:graphics.swapchain;
import std;
import :win32;
import :core;
import :com;
import :math;
import :graphics.graphicsresource;

export namespace dx3d
{
	struct SwapChainDesc
	{
		void* WinHandle = nullptr;
		Rect WinSize = {};
	};
	class SwapChain final : public GraphicsResource
	{
	public:
		explicit SwapChain(const SwapChainDesc& desc, const GraphicsResourceDesc& base)
			: GraphicsResource{ base }
		{
			// https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ns-dxgi-dxgi_swap_chain_desc
			auto swapchainDesc = DXGI::DXGI_SWAP_CHAIN_DESC{
				.BufferDesc = {
					.Width = std::max(desc.WinSize.Width, 1u),
					.Height = std::max(desc.WinSize.Height, 1u),
					.Format = DXGI::DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM
				},
				.SampleDesc = {
					.Count = 1,
				},
				.BufferUsage = DXGI::Usage::RenderTargetOutput,
				.BufferCount = 2,
				.OutputWindow = static_cast<Win32::HWND>(desc.WinHandle),
				.Windowed = Win32::True,
				.SwapEffect = DXGI::DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD,
			};

			auto hr = HResult{
				factory.CreateSwapChain(
					&device,
					&swapchainDesc,
					swapchain.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw ComError{ hr, "Failed to create swap chain" };
		}
	private:
		Ptr<DXGI::IDXGISwapChain> swapchain;
	};
}
