export module dx3d:graphics.graphicsresource;
import std;

export namespace dx3d
{
	class GraphicsResource : public Base
	{
	public:
		virtual ~GraphicsResource() override = default;
		explicit GraphicsResource(const BaseDesc& desc)
			: Base(desc)
		{}
	};
}