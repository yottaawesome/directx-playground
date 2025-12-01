export module shared:window;
import std;
import :win32;

export namespace Raii
{
	template<auto VDeleteFn>
	struct Deleter
	{
		static constexpr void operator()(auto ptr)
		{
			VDeleteFn(ptr);
		}
	};
}

export namespace UI
{
	using HwndUniquePtr = std::unique_ptr<std::remove_pointer_t<Win32::HWND>, Raii::Deleter<Win32::DestroyWindow>>;

	template<Win32::DWORD VMessageId>
	struct Message
	{
		Win32::HWND hwnd = nullptr;
		unsigned uMsg = 0;
		Win32::WPARAM wParam = 0;
		Win32::LPARAM lParam = 0;
	};

	template<typename T, Win32::DWORD VMsgId>
	concept HandlesMessage = requires(T t)
	{
		t.HandleMessage(Message<VMsgId>{});
	};

	struct Window
	{
		HwndUniquePtr window;

		auto Initialise(this auto& self) -> void
		{
			self.RegisterClass();
		}

		void RegisterClass(this Window& self)
		{
			Win32::WNDCLASSW wc{
				.lpfnWndProc = WindowProc<std::remove_reference_t<decltype(self)>>,
				.hInstance = Win32::GetModuleHandleW(nullptr),
				.lpszClassName = L"Main D3D12 Window Class",
			};
		}

		void CreateWindow(this Window& self, int width, int height)
		{
			Win32::CreateWindowExW(
				0,
				L"Main D3D12 Window Class",
				L"D3D12 Window",
				Win32::WindowStyles::WsOverlappedWindow | Win32::WindowStyles::WsVisible,
				Win32::CwUseDefault,
				Win32::CwUseDefault,
				width,
				height,
				nullptr,
				nullptr,
				Win32::GetModuleHandleW(nullptr),
				reinterpret_cast<Win32::LPVOID>(&self) // pass 'this' pointer for use in WM_NCCREATE
			);
		}

		static constexpr std::array HandledMessages{
			Win32::Messages::Destroy,
			Win32::Messages::Paint,
			Win32::Messages::KeyUp,
			Win32::Messages::Command,
			Win32::Messages::NonClientDestroy
		};

		template<typename TWindow>
		static auto WindowProc(Win32::HWND hwnd, unsigned uMsg, Win32::WPARAM wParam, Win32::LPARAM lParam) -> Win32::LRESULT
		{
			TWindow* pThis = nullptr;

			if (uMsg == Win32::Messages::NonClientCreate)
			{
				auto* pCreate = reinterpret_cast<Win32::CREATESTRUCT*>(lParam);
				pThis = reinterpret_cast<TWindow*>(pCreate->lpCreateParams);
				Win32::SetWindowLongPtrW(hwnd, Win32::Gwlp_UserData, reinterpret_cast<Win32::LONG_PTR>(pThis));

				// Adopt ownership once, during creation
				pThis->window = HwndUniquePtr(hwnd);
			}
			else
			{
				pThis = reinterpret_cast<TWindow*>(Win32::GetWindowLongPtrW(hwnd, Win32::Gwlp_UserData));

				// Detach before the OS destroys the HWND to avoid double-destroy later.
				// Caught by AI, more information here https://learn.microsoft.com/en-us/cpp/mfc/tn017-destroying-window-objects?view=msvc-170
				// under the "Auto cleanup with CWnd::PostNcDestroy" header.
				if (pThis and uMsg == Win32::Messages::NonClientDestroy)
				{
					Win32::SetWindowLongPtrW(hwnd, Win32::Gwlp_UserData, 0);
					pThis->window.release();
				}
			}

			if (not pThis)
				Win32::DefWindowProcW(hwnd, uMsg, wParam, lParam);

			return [=]<size_t...Indices>(std::index_sequence<Indices...>)
			{
				Win32::LRESULT returnValue = 0;
				bool handled = 
					([&] constexpr -> bool
					{
						if constexpr (HandlesMessage<TWindow, HandledMessages[Indices]>)
						{
							returnValue = pThis->HandleMessage(
								Message<HandledMessages[Indices]>{
									.hwnd = hwnd,
									.uMsg = uMsg,
									.wParam = wParam,
									.lParam = lParam
								});
							return true;
						}
						return false;
					}() or ...);
				return handled
					? returnValue
					: Win32::DefWindowProcW(hwnd, uMsg, wParam, lParam);
			}(std::make_index_sequence<HandledMessages.size()>{});
		}
	};
}