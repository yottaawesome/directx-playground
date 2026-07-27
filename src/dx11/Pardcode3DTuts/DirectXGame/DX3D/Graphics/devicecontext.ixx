export module dx3d:graphics.devicecontext;
import std;
import :core;
import :com;
import :win32;
import :graphics.graphicsresource;
import :graphics.swapchain;
import :math;

export namespace dx3d
{
	class DeviceContext final : public GraphicsResource
	{
	public:
		virtual ~DeviceContext() override = default;
		explicit DeviceContext(const GraphicsResourceDesc& desc)
			: GraphicsResource(desc)
		{
			auto hr = device.CreateDeferredContext(0, deferredDeviceContext.ReleaseAndGetAddressOf());
			if (Win32::Failed(hr))
				throw ComError{ hr, "Failed to create deferred device context." };
		}
		void ClearAndSetBackBuffer(const SwapChain& swapChain, const Vec4& color)
		{
			auto rtv = swapChain.renderTargetView.Get();
			deferredDeviceContext->ClearRenderTargetView(rtv, color.Data());
			deferredDeviceContext->OMSetRenderTargets(1, &rtv, nullptr);
		}
	private:
		friend class GraphicsDevice;
		Ptr<D3D11::ID3D11DeviceContext> deferredDeviceContext; //m_context
	};
}
