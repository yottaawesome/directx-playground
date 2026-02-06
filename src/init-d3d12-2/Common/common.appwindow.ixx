export module initd3d12x:common.appwindow;
import std;
import shared;

export namespace Common
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

		~AppWindow()
		{
			if (window)
			{
				window.reset();
				Win32::UnregisterClassW(GetWindowClass().data(), Win32::GetModuleHandleW(nullptr));
			}
		}

		AppWindow(TMainApp* mainApp, std::uint32_t width, std::uint32_t height)
			: mainApp(mainApp), width(width), height(height)
		{ }

		AppWindow(const AppWindow&) = delete;
		AppWindow& operator=(const AppWindow&) = delete;

		AppWindow(AppWindow&& other) { Move(other); };
		auto operator=(this AppWindow& self, AppWindow&& other) -> AppWindow& 
		{ 
			self.Move(other); 
			return self; 
		};

		void Initialise(this AppWindow& self)
		{
			self.RegisterClass();
			try
			{
				self.CreateWindow(self.width, self.height);
			}
			catch(...)
			{
				Win32::UnregisterClassW(self.GetWindowClass().data(), Win32::GetModuleHandleW(nullptr));
				throw;
			}
		}

		auto GetHeight(this const AppWindow& self) noexcept -> std::uint32_t
		{
			if (not self.window)
				return self.height;
			auto clientRect = Win32::RECT{};
			Win32::GetClientRect(self.window.get(), &clientRect);
			return clientRect.bottom - clientRect.top;
		}

		auto GetWidth(this const AppWindow& self) noexcept -> std::uint32_t
		{
			if (not self.window)
				return self.width;
			auto clientRect = Win32::RECT{};
			Win32::GetClientRect(self.window.get(), &clientRect);
			return clientRect.right - clientRect.left;
		}

		auto GetHandle(this AppWindow& self) -> Win32::HWND
		{
			return self.window.get();
		}

		constexpr auto GetWindowTitle(this const auto&) noexcept -> std::wstring_view
		{
			return L"D3D12 Window";
		}

		constexpr auto GetWindowClass(this const auto&) noexcept -> std::wstring_view
		{
			return L"Main D3D12 Window Class";
		}

	private:
		TMainApp* mainApp = nullptr;
		Shared::HwndUniquePtr window;
		std::uint32_t width = 0;
		std::uint32_t height = 0;

	private:
		void Move(this AppWindow& self, AppWindow& other)
		{
			if (&self == &other)
				return;

			self.window = std::move(other.window);
			self.width = other.width;
			self.height = other.height;
			self.mainApp = other.mainApp;

			other.mainApp = nullptr;
			other.width = 0;
			other.height = 0;

			if (self.window)
			{
				Win32::SetWindowLongPtrW(
					self.window.get(),
					Win32::Gwlp_UserData,
					reinterpret_cast<Win32::LONG_PTR>(&self)
				);
			}
		}

		void CreateWindow(this AppWindow& self, int width, int height)
		{
			Win32::CreateWindowExW(
				0,
				self.GetWindowClass().data(),
				self.GetWindowTitle().data(),
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
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to create window" };

			Win32::ShowWindow(self.window.get(), Win32::ShowWindowOptions::ShowNormal);
		}

		void RegisterClass(this auto& self)
		{
			auto classEx = Win32::WNDCLASSEXW{ self.GetClass() };
			classEx.lpfnWndProc = WindowProc<AppWindow<TMainApp>>;
			if (not Win32::RegisterClassExW(&classEx))
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to register window class, please ensure it's not already registered by another window." };
		}

		auto GetClass(this const AppWindow& self) -> Win32::WNDCLASSEXW
		{
			return Win32::WNDCLASSEXW{
				.cbSize = sizeof(Win32::WNDCLASSEXW),
				.hInstance = Win32::GetModuleHandleW(nullptr),
				.hIcon = Win32::LoadIconW(nullptr, Win32::IdiApplication),
				.hCursor = Win32::LoadCursorW(nullptr, Win32::IdcArrow),
				.hbrBackground = static_cast<Win32::HBRUSH>(Win32::GetStockObject(Win32::Brushes::White)),
				.lpszClassName = self.GetWindowClass().data(),
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
				if (pThis and uMsg == Win32::Messages::NonClientDestroy and pThis->window.get() == hwnd)
				{
					// Only release if the message is for the currently managed window
					Win32::SetWindowLongPtrW(hwnd, Win32::Gwlp_UserData, 0);
					pThis->window.release();
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