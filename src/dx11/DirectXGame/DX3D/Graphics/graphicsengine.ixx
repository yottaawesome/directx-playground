export module dx3d:graphics.graphicsengine;
import std;
import :core;
import :graphics.rendersystem;

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
			renderSystem = std::make_unique<RenderSystem>(
				RenderSystemDesc{ 
					.Base = desc.Base 
				});
		}
	private:
		std::unique_ptr<RenderSystem> renderSystem;
	};
}