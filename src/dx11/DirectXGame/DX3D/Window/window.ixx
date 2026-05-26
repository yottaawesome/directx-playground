export module dx3d:window;
import std;
import :core;
import :win32;

export namespace dx3d
{
	class Window : public Base
	{
	public:
		virtual ~Window() override
		{
			Win32::DestroyWindow(hwnd);
		}

		Window()
		{
			Init();
			
		}

	private:
		Win32::HWND hwnd = nullptr;

		static constexpr auto GetWindowClassName() noexcept -> const wchar_t*
		{
			return L"DX3DWindow";
		}

		static auto WindowProc(Win32::HWND hWnd, Win32::UINT msg, Win32::WPARAM wParam, Win32::LPARAM lParam) -> Win32::LRESULT
		{
			switch (msg)
			{
			case Win32::Messages::Close:
				Win32::PostQuitMessage(0);
				return 0;
			default:
				return Win32::DefWindowProcW(hWnd, msg, wParam, lParam);
			}
		}

		void Init()
		{
			static auto isRegistered =
				[] static -> bool
				{
					auto wndClass = Win32::WNDCLASSEXW{
						.cbSize = sizeof(Win32::WNDCLASSEXW),
						.lpfnWndProc = &WindowProc,
						.lpszClassName = GetWindowClassName(),
					};
					if (not Win32::RegisterClassExW(&wndClass))
					{
						// TODO: improve error handling
						throw std::runtime_error{ "Failed to register window class" };
					}
					return true;
				}();

			hwnd = 
				[] static -> Win32::HWND
				{
					constexpr auto WindowStyles =
						Win32::WindowStyles::Overlapped | Win32::WindowStyles::Caption | Win32::WindowStyles::Visible | Win32::WindowStyles::SysMenu;

					auto rect = Win32::RECT{ 0, 0, 1280, 720 };
					Win32::AdjustWindowRect(&rect, WindowStyles, Win32::False);

					auto hwnd =
						Win32::CreateWindowExW(
							0,
							GetWindowClassName(),
							L"DX3D Window",
							WindowStyles,
							Win32::CwUseDefault,
							Win32::CwUseDefault,
							rect.right - rect.left,
							rect.bottom - rect.top,
							nullptr,
							nullptr,
							nullptr,
							nullptr
						);
					// TODO: improve error handling
					if (not hwnd)
					{
						throw std::runtime_error{ "Failed to create window" };
					}
					return hwnd;
				}();
		}
	};
}