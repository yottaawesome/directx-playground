export module dx3d:graphics.swapchain;
import std;
import :core;

export namespace dx3d
{
	struct SwapChainDesc
	{
		BaseDesc Base;
	};
	class SwapChain final : public Base
	{
	public:
		virtual ~SwapChain() override = default;
		explicit SwapChain(const SwapChainDesc& desc)
			: Base(desc.Base)
		{}
	private:
	};
}
