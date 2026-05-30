export module dx3d:game.display;
import std;
import :core;
import :window;
import :graphics;
import :math;

namespace dx3d
{
	struct DisplayDesc
	{
		WindowDesc Window;
		RenderSystem& RenderSystem;
	};
	class Display final : public Window
	{
	public:
		explicit Display(const DisplayDesc& desc)
			: Window(desc.Window)
		{
			swapchain = desc.RenderSystem.CreateSwapChain(
				{
					.WinHandle = hwnd,
					.WinSize = size
				});
		}
	private:
		std::shared_ptr<SwapChain> swapchain;
	};
}
