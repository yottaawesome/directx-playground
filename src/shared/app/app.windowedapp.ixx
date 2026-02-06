export module shared:app.windowedapp;
import std;
import :error;
import :raii;
import :win32;
import :app.common;

export namespace App
{
	struct WindowedApp
	{
		virtual ~WindowedApp() = default;
		WindowedApp(const WindowedApp&) = delete;
		WindowedApp& operator=(const WindowedApp&) = delete;

		WindowedApp(WindowedApp&& other) 
		{
			Move(other);
		};
		auto operator=(this auto&& self, WindowedApp&& other) -> decltype(auto)
		{
			self.Move(other);
			return decltype(self)(self);
		};

		constexpr WindowedApp(unsigned width, unsigned height)
			: width(width), height(height)
		{
			// Warning: if you initialise here, the correct explicit object type
			// won't be used, meaning OnMessage() will fail to be invoked in
			// subclasses that define OnMessage().
			//if consteval { }
			//else { Initialise(); }
		}

		auto InitialiseWindow(this auto& self) -> void
		{
			self.RegisterClass();
			self.CreateWindow(800, 600);
		}

		void RegisterClass(this auto& self)
		{
			auto classEx = self.GetClass();
			classEx.lpfnWndProc = WindowProc<std::remove_cvref_t<decltype(self)>>;
			Win32::ATOM atom = Win32::RegisterClassExW(&classEx);
			if (atom == 0)
			{
				auto lastError = Win32::GetLastError();
				throw Error::Win32Error{ lastError, "Failed to register window class" };
			}
		}

		auto GetClass(this const auto& self) -> Win32::WNDCLASSEXW
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

		void CreateWindow(this auto& self, int width, int height)
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

		auto GetHandle(this auto&& self) -> Win32::HWND
		{
			return self.window.get();
		}

		void Move(this auto&& self, WindowedApp& other)
		{
			if (&self == &other)
				return;

			if (self.window)
				Win32::SetWindowLongPtrW(self.window.get(), Win32::Gwlp_UserData, 0);

			self.window = std::move(other.window);
			self.width = other.width;
			self.height = other.height;

			if (self.window)
				Win32::SetWindowLongPtrW(
					self.window.get(),
					Win32::Gwlp_UserData,
					reinterpret_cast<Win32::LONG_PTR>(&self)
				);
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

		// Any OnMessage() calls here won't be invoked unless the subclass pulls them 
		// into scope with using Shared::WindowedApp::OnMessage().

		//
		// Called by WindowProc, which then dispatches the message to either the generic handler
		// or specific handlers by subclasses.
		auto HandleMessage(
			this auto&& self,
			Win32::HWND hwnd,
			unsigned msgType,
			Win32::WPARAM wParam,
			Win32::LPARAM lParam
		) -> Win32::LRESULT
		{
			return [=, &self]<size_t...Is>(std::index_sequence<Is...>)
			{
				Win32::LRESULT result;
				bool handled = (... or
					[=, &self, &result]<typename TMsg = Win32Message<HandledMessages[Is]>>
					{
						if constexpr (Handles<std::remove_cvref_t<decltype(self)>, TMsg>)
							return TMsg::uMsg == msgType ? (result = self.OnMessage(TMsg{ hwnd, wParam, lParam }), true) : false;
						return false;
					}());
				if (handled)
					return result;
				return msgType == Win32::Messages::Destroy
					? (Win32::PostQuitMessage(0), 0)
					: Win32::DefWindowProcW(hwnd, msgType, wParam, lParam);
			}(std::make_index_sequence<HandledMessages.size()>());
		}

		auto GetDimensions(this const auto& self) noexcept -> Dimensions
		{
			return Dimensions{ self.width, self.height };
		}

		auto GetHeight(this const auto& self) noexcept -> unsigned
		{
			return self.height;
		}

		auto GetWidth(this const auto& self) noexcept -> unsigned
		{
			return self.width;
		}

	private:
		HwndUniquePtr window;
		unsigned width = 0;
		unsigned height = 0;
	};
}