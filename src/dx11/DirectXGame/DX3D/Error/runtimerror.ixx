export module dx3d:error.runtimerror;
import std;

export namespace dx3d
{
	enum class ErrorExitCode
	{
		GeneralFailure = 1,
		AssetsNotFound = 2,
	};

	class RuntimeError : public std::runtime_error
	{
	public:
		RuntimeError(
			std::string_view msg,
			std::optional<ErrorExitCode> exitCode = std::nullopt,
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& trace = std::stacktrace::current()
		) : std::runtime_error(Format(msg, loc, trace)), ExitCode(exitCode)
		{}

		static auto Format(
			std::string_view message,
			const std::source_location& loc,
			const std::stacktrace& trace
		) -> std::string
		{
			return std::format("{} at {} of {}:{}\n{}", message, loc.function_name(), loc.line(), loc.file_name(), trace);
		}

		std::optional<ErrorExitCode> ExitCode = std::nullopt;
	};
}
