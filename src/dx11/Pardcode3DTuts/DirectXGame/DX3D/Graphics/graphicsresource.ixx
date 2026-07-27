export module dx3d:graphics.graphicsresource;
import std;
import :win32;
import :core;

export namespace dx3d
{
	struct GraphicsResourceDesc
	{
		BaseDesc Base;
		D3D11::ID3D11Device& Device;
		DXGI::IDXGIFactory& Factory;
	};
	class GraphicsResource : public Base
	{
	public:
		virtual ~GraphicsResource() override = default;
		explicit GraphicsResource(const GraphicsResourceDesc& desc)
			: Base(desc.Base), device(desc.Device), factory(desc.Factory)
		{}
	protected:
		D3D11::ID3D11Device& device;
		DXGI::IDXGIFactory& factory;
	};
}