export module dx3d:error.win32error;
import std;
import :win32;
import :error.runtimerror;
import :error.functions;

export namespace dx3d
{
	class Win32Error : public RuntimeError
	{
	public:
		Win32Error(
			Win32::DWORD code,
			std::string_view message,
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& trace = std::stacktrace::current()
		) : RuntimeError(FormatPrefix(message, code), std::nullopt, loc, trace)
		{}

	private:
		static auto FormatPrefix(std::string_view message, Win32::DWORD code) -> std::string
		{
			return std::format("{}: {}", message, GetErrorMessage(code));
		}
	};
}