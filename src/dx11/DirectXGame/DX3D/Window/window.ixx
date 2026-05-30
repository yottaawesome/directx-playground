export module dx3d:window;
import std;
import :core;
import :win32;
import :math;

export namespace dx3d
{
	struct WindowDesc
	{
		BaseDesc Base;
		Rect Size;
	};
	class Window : public Base
	{
	public:
		virtual ~Window() override
		{
			Win32::DestroyWindow(hwnd);
		}

		explicit Window(const WindowDesc& desc)
			: Base(desc.Base), size{ desc.Size }
		{
			Init();
		}

	protected:
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
				[size = size] -> Win32::HWND
				{
					constexpr auto WindowStyles =
						Win32::WindowStyles::Overlapped | Win32::WindowStyles::Caption | Win32::WindowStyles::Visible | Win32::WindowStyles::SysMenu;

					auto rect = Win32::RECT{ 0, 0, static_cast<Win32::LONG>(size.Width), static_cast<Win32::LONG>(size.Height) };
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
	protected:
		Win32::HWND hwnd = nullptr;
		Rect size{ 1280, 720 };
	};
}