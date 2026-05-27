export module dx3d:graphics.graphicscontext;
import std;
import :core;
import :graphics.rendersystem;

export namespace dx3d
{
	class Graphics final : public Base
	{
	public:
		virtual ~Graphics() override = default;
		Graphics()
		{

		}
	};
}