export module initd3d12x:appwindow;
import std;
import shared;

export namespace Test
{
	template<typename T, typename M>
	concept Handles = requires(T t, M m)
	{
		{ t.OnMessage(m) } -> std::convertible_to<Win32::LRESULT>;
	};

	template<typename T>
	concept IsClass = std::is_class_v<T>;

	template<IsClass TMainApp>
	class AppWindow
	{
	public:
		using TAppType = TMainApp;

		AppWindow(TMainApp* mainApp, unsigned width, unsigned height)
			: mainApp(mainApp), width(width), height(height)
		{
		}

		auto Initialise(this AppWindow& self) -> void
		{
			self.RegisterClass();
			self.CreateWindow(800, 600);
		}

		auto GetHeight(this const AppWindow& self) noexcept -> unsigned
		{
			return self.height;
		}

		auto GetWidth(this const AppWindow& self) noexcept -> unsigned
		{
			return self.width;
		}

		auto GetHandle(this AppWindow& self) -> Win32::HWND
		{
			return self.window.get();
		}

	private:
		TMainApp* mainApp = nullptr;
		Shared::HwndUniquePtr window;
		unsigned width = 0;
		unsigned height = 0;

	private:
		void CreateWindow(this AppWindow& self, int width, int height)
		{
			HWND hwnd = Win32::CreateWindowExW(
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
			if (not self.window)
				throw std::runtime_error("Failed to create window");

			Win32::ShowWindow(self.window.get(), Win32::ShowWindowOptions::ShowNormal);
		}

		void RegisterClass(this AppWindow& self)
		{
			auto classEx = Win32::WNDCLASSEXW{ self.GetClass() };
			classEx.lpfnWndProc = WindowProc<AppWindow<TMainApp>>;
			if (not Win32::RegisterClassExW(&classEx))
			{
				auto lastError = Win32::GetLastError();
				throw Error::Win32Error{ lastError, "Failed to register window class" };
			}
		}

		auto GetClass(this const AppWindow& self) -> Win32::WNDCLASSEXW
		{
			return Win32::WNDCLASSEXW{
				.cbSize = sizeof(Win32::WNDCLASSEXW),
				.hInstance = Win32::GetModuleHandleW(nullptr),
				.hIcon = Win32::LoadIconW(nullptr, Win32::IdiApplication),
				.hCursor = Win32::LoadCursorW(nullptr, Win32::IdcArrow),
				.hbrBackground = static_cast<Win32::HBRUSH>(Win32::GetStockObject(Win32::Brushes::White)),
				.lpszClassName = L"Main D3D12 Window Class",
			};
		}

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
				pThis->window = Shared::HwndUniquePtr(hwnd);
			}
			else
			{
				pThis = reinterpret_cast<TWindow*>(Win32::GetWindowLongPtrW(hwnd, Win32::Gwlp_UserData));

				// Detach before the OS destroys the HWND to avoid double-destroy later.
				// Caught by AI, more information here https://learn.microsoft.com/en-us/cpp/mfc/tn017-destroying-window-objects?view=msvc-170
				// under the "Auto cleanup with CWnd::PostNcDestroy" header.
				if (pThis and uMsg == Win32::Messages::NonClientDestroy)
				{
					// Only release if the message is for the currently managed window
					if (pThis->window.get() == hwnd)
					{
						Win32::SetWindowLongPtrW(hwnd, Win32::Gwlp_UserData, 0);
						pThis->window.release();
					}
				}
			}

			return pThis
				? pThis->HandleMessage(hwnd, uMsg, wParam, lParam)
				: Win32::DefWindowProcW(hwnd, uMsg, wParam, lParam);
		}
		
		constexpr auto Dispatch(this auto& self, unsigned msgType, auto msg) -> std::pair<bool, Win32::LRESULT>
			requires Handles<TMainApp, decltype(msg)>
		{
			return msg.uMsg == msgType ? std::pair{ true, self.mainApp->OnMessage(msg) } : std::pair{ false, Win32::LRESULT{} };
		}

		constexpr auto Dispatch(this auto& self, unsigned msgType, auto msg) -> std::pair<bool, Win32::LRESULT>
			requires (not Handles<TMainApp, decltype(msg)>)
		{
			return { false, 0 };
		}

		auto HandleMessage(
			this auto& self,
			Win32::HWND hwnd,
			unsigned msgType,
			Win32::WPARAM wParam,
			Win32::LPARAM lParam
		) -> Win32::LRESULT
		{
			return[=, &self]<size_t...Is>(std::index_sequence<Is...>)
			{
				auto result = std::pair<bool, Win32::LRESULT>{};
				(... or [=, &self, &result]<typename TMsg = Shared::Win32Message<Shared::HandledMessages[Is]>>
				{
					if constexpr (Handles<TMainApp, TMsg>)
					{
						// The original code used to dispatch OnMessage to this type, and the below works for that
						// but does not work if we're dispatching to a different type (TMainApp), as it appears
						// the compiler expects OnMessage to exist on TMainApp. We work around this by using the
						// Dispatch() helper, which uses requires to conditionally invoke OnMessage only if it 
						// exists on TMainApp for the given message type. Note that using an inline constexpr 
						// requires check didn't help, as the compiler still expected OnMessage for some reason.
						//return TMsg::uMsg == msgType ? (result.second = self.mainApp->OnMessage(TMsg{ hwnd, wParam, lParam }), true) : false;
						result = self.Dispatch(msgType, TMsg{ hwnd, wParam, lParam });
						return result.first;
					}
					return false;
				}());
				if (result.first)
					return result.second;
				return msgType == Win32::Messages::Destroy
					? (Win32::PostQuitMessage(0), 0)
					: Win32::DefWindowProcW(hwnd, msgType, wParam, lParam);
			}(std::make_index_sequence<Shared::HandledMessages.size()>());
		}
	};
}