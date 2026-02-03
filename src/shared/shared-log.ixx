export module shared:log;
import std;
import :strings;

export namespace Log
{
	template<Strings::FixedString VLevel, typename...TArgs>
	inline constexpr void LogMessage(std::format_string<TArgs...> message, TArgs&&...args) noexcept
	{
		std::println("{} [{}] {}", std::chrono::system_clock::now(), VLevel.ToView(), std::format(message, std::forward<TArgs>(args)...));
	}

	template<typename...TArgs>
	inline constexpr void Info(std::format_string<TArgs...> message, TArgs&&...args) noexcept
	{
		std::println("{} [Info] {}", std::chrono::system_clock::now(), std::format(message, std::forward<TArgs>(args)...));
	}
	template<typename...TArgs>
	inline constexpr void Warn(std::format_string<TArgs...> message, TArgs&&...args) noexcept
	{
		std::println("{} [Warn] {}", std::chrono::system_clock::now(), std::format(message, std::forward<TArgs>(args)...));
	}
	template<typename...TArgs>
	inline constexpr void Error(std::format_string<TArgs...> message, TArgs&&...args) noexcept
	{
		std::println("{} [Error] {}", std::chrono::system_clock::now(), std::format(message, std::forward<TArgs>(args)...));
	}
}
