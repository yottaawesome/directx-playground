export module initd3d12x:approach1.d3d12xapp;
import std;
import shared;
import :approach1.d3d12context;

// Approach 1: The D3D12 state is encapsulated in a separate class, and the application class 
// holds an instance of it.
export namespace Approach1
{
	class D3D12xApp
	{
	public:
		D3D12xApp()
		{
			appWindow.Initialise();
			d3d12State.emplace(WindowView{ .Handle = appWindow.GetHandle() });
			d3d12State->Initialise();
		}

		auto OnMessage(this D3D12xApp& self, const Shared::Win32Message<Win32::Messages::Size>& msg) -> Win32::LRESULT
		{
			if (self.d3d12State)
				self.d3d12State->Resize(Win32::LoWord(msg.lParam), Win32::HiWord(msg.lParam));
			return 0;
		}

		void OnIdle(this auto& self)
		{
			// Application idle processing goes here
		}

		auto MainLoop(this auto&& self) -> Win32::LRESULT
		{
			auto msg = Win32::MSG{};
			while (msg.message != Win32::Messages::Quit)
			{
				if (Win32::PeekMessageW(&msg, self.appWindow.GetHandle(), 0, 0, Win32::PeekMessageOptions::Remove))
				{
					Win32::TranslateMessage(&msg);
					Win32::DispatchMessageW(&msg);
				}
				else
				{
					self.OnIdle();
				}
			}
			return msg.wParam;
		}
	private:
		using Window = Common::AppWindow<D3D12xApp>;
		Window appWindow{ this, 800, 600 };
		std::optional<D3d12Context> d3d12State;
	};
}
