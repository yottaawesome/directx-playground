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
			appWindow.SetCallback(
				[this](const App::Win32Message<Win32::Messages::Size>& msg)
				{ 
					return this->OnMessage(msg); 
				});
			appWindow.InitialiseWindow();
			d3d12State.emplace(WindowView{ .Handle = appWindow.GetHandle() });
			d3d12State->Initialise();
		}

		// Prevent moving to avoid dangling 'this' pointer inside appWindow
		D3D12xApp(D3D12xApp&&) = delete;
		D3D12xApp& operator=(D3D12xApp&&) = delete;

		auto OnMessage(this D3D12xApp& self, const App::Win32Message<Win32::Messages::Size>& msg) -> Win32::LRESULT
		{
			// SIZE_MINIMIZED = 1. If minimized, dimensions are usually 0, and we shouldn't resize buffers.
			if (not self.d3d12State or msg.wParam == 1)
				return 0;

			auto width = Win32::LoWord(msg.lParam);
			auto height = Win32::HiWord(msg.lParam);
			if (width > 0 and height > 0)
				self.d3d12State->Resize(width, height);
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
		using Window = App::WindowedApp2;
		Window appWindow{ 800, 600 };
		std::optional<D3d12Context> d3d12State;
	};
}
