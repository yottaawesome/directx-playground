export module shared:d3d12app;
import :window;
import :win32;
import std;

export namespace Shared
{
	struct D3D12App 
	{
		auto MainLoop(this auto&& self) -> Win32::LRESULT
		{
			Win32::MSG msg{};
			while (msg.message != Win32::Messages::Quit)
			{
				if (Win32::PeekMessageW(&msg, self.window.GetHandle(), 0, 0, Win32::PeekMessageOptions::Remove))
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

		void Initialise(this const D3D12App& self) { }

		void OnIdle(this const D3D12App&& self) { }
	};
}
