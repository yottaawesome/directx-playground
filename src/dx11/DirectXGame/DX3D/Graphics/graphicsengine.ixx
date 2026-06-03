export module dx3d:graphics.graphicsengine;
import std;
import :core;
import :graphics.graphicsdevice;
import :graphics.devicecontext;
import :graphics.swapchain;

export namespace dx3d
{
	struct GraphicsEngineDesc
	{
		BaseDesc Base;
	};
	class GraphicsEngine final : public Base
	{
	public:
		virtual ~GraphicsEngine() override = default;
		explicit GraphicsEngine(const GraphicsEngineDesc& desc)
			: Base(desc.Base)
		{
			graphicsDevice = std::make_unique<GraphicsDevice>(
				GraphicsDeviceDesc{ 
					.Base = desc.Base 
				});
			auto& device = GetGraphicsDevice();
			deferredDeviceContext = device.CreateDeviceContext();
		}
		auto GetGraphicsDevice() noexcept -> GraphicsDevice&
		{
			return *graphicsDevice;
		}
		void Render(SwapChain& swapChain)
		{
			deferredDeviceContext->ClearAndSetBackBuffer(swapChain, {1,0,0,1});
			graphicsDevice->ExecuteCommandLists(*deferredDeviceContext);
			swapChain.Present();
		}
	private:
		std::unique_ptr<GraphicsDevice> graphicsDevice;
		std::shared_ptr<DeviceContext> deferredDeviceContext;
	};
}