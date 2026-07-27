export module dx3d:com.error;
import std;
import :win32;
import :error;
import :com.hresult;

export namespace dx3d
{
	class ComError : public RuntimeError
	{
	public:
		ComError(
			Win32::HRESULT hr,
			std::string_view message,
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& trace = std::stacktrace::current()
		) : RuntimeError(FormatPrefix(message, hr), std::nullopt, loc, trace)
		{}

	private:
		static auto FormatPrefix(std::string_view message, Win32::HRESULT hr) -> std::string
		{
			return std::format("{}: {} (0x{:08X})", message, HResultToString(hr), static_cast<unsigned long>(hr));
		}
	};
}