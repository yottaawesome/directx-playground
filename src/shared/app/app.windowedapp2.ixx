export module shared:app.windowedapp2;
import std;
import :error;
import :raii;
import :win32;
import :app.common;
import :app.windowedapp;

export namespace App
{
	template<typename TMessage>
	using Callback = std::function<auto(const TMessage&)->Win32::LRESULT>;

	constexpr auto DefaultCallback = 
		[](const auto& msg) static -> Win32::LRESULT 
		{
			return Win32::DefWindowProcW(msg.hwnd, msg.uMsg, msg.wParam, msg.lParam);
		};

	struct WindowedApp2 : WindowedApp
	{
		virtual ~WindowedApp2() = default;
		WindowedApp2(const WindowedApp2&) = delete;
		WindowedApp2& operator=(const WindowedApp2&) = delete;

		WindowedApp2(unsigned width, unsigned height)
			: WindowedApp(width, height)
		{ }

		auto OnMessage(this WindowedApp2& self, const auto& msg) -> Win32::LRESULT
		{
			auto result = std::pair<bool, Win32::LRESULT>{};
			std::apply(
				[&result, &msg](auto&&... args)
				{
					(... or [&result, &msg, &args]
					{
						if constexpr (std::invocable<decltype(args), decltype(msg)>)
							result = std::pair{ true, std::invoke(args, msg) };
						return result.first;
					}());
				},
				self.Callbacks
			);
			if (auto [handled, value] = result; handled)
				return value;

			return msg.uMsg == Win32::Messages::Destroy
				? (Win32::PostQuitMessage(0), 0)
				: Win32::DefWindowProcW(msg.hwnd, msg.uMsg, msg.wParam, msg.lParam);
		}

		std::tuple<
			Callback<Win32Message<Win32::Messages::Size>>
		> Callbacks{
			DefaultCallback
		};

		constexpr void SetCallback(this auto& self, auto&& newCallback)
		{
			// At first blush, this would appear to not work, but I tested 
			// this on both Clang 21.1 and GCC 15.2 on Godbolt, and they
			// accepted this code.
			static_assert(
				[&newCallback]<typename...TArgs>(std::tuple<TArgs...>& allCallbacks) constexpr
				{
					return ([&oldCallback = std::get<TArgs>(allCallbacks), &newCallback] constexpr
					{
						return std::assignable_from<decltype(oldCallback), decltype(newCallback)>;
					}() or ...);
				}(self.Callbacks),
				"Callback type not supported. Please check your parameters and update the Callbacks member if supporting a new message type."
			);

			[&newCallback]<typename...TArgs>(std::tuple<TArgs...>& allCallbacks) constexpr
			{
				bool applied = 
					([&oldCallback = std::get<TArgs>(allCallbacks), &newCallback] constexpr
					{
						if constexpr (std::assignable_from<decltype(oldCallback), decltype(newCallback)>)
						{
							oldCallback = std::forward<decltype(newCallback)>(newCallback);
							return true;
						}
						return false;
					}() or ...);
			}(self.Callbacks);
		}
	};
}
