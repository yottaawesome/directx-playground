export module shared:error;
import std;
import :win32;

export namespace Error
{
	auto TranslateErrorCode(Win32::DWORD errorCode) -> std::string
	{
		if (errorCode == 0)
			return {};

		constexpr auto options = Win32::FormatMessageOptions::AllocateBuffer 
			| Win32::FormatMessageOptions::FromSystem 
			| Win32::FormatMessageOptions::IgnoreInserts;

		auto messageBuffer = static_cast<char*>(nullptr);
		auto size = size_t{
			Win32::FormatMessageA(
				options,
				nullptr,
				errorCode,
				0,
				reinterpret_cast<char*>(&messageBuffer),
				0,
				nullptr
			)};
		if (size == 0)
			return std::format("Unknown error code: {}", errorCode);

		auto message = std::string{ messageBuffer, size };
		Win32::LocalFree(messageBuffer);

		return message;
	}

	template<typename TDummy>
	struct Error : std::runtime_error
	{
		explicit Error(
			std::string_view message,
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& trace = std::stacktrace::current()
		) : std::runtime_error(Format(message, loc, trace))
		{ }

		static auto Format(
			std::string_view message,
			const std::source_location& loc,
			const std::stacktrace& stacktrace
		) -> std::string
		{
			return std::format(
				"{}\n"
				"  at {} ({}:{})\n"
				"Stacktrace:\n{}",
				loc.function_name(),
				loc.file_name(),
				loc.line(),
				stacktrace
			);
		}
	};

	class Win32Error : std::runtime_error
	{
	public:
		explicit Win32Error(
			Win32::DWORD errorCode,
			std::string_view message, 
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& trace = std::stacktrace::current()
		) : m_errorCode(errorCode), std::runtime_error(Format(errorCode, message, loc, trace))
		{ }

		auto GetErrorCode(this const Win32Error& self) noexcept -> Win32::DWORD
		{
			return self.m_errorCode;
		}

	private:
		Win32::DWORD m_errorCode;

		static auto Format(
			Win32::DWORD errorCode, 
			std::string_view message, 
			const std::source_location& loc, 
			const std::stacktrace& stacktrace
		) -> std::string
		{
			return std::format(
				"Win32 Error (code {}): {}\n"
				"  at {} ({}:{})\n"
				"Stacktrace:\n{}",
				errorCode,
				TranslateErrorCode(errorCode),
				loc.function_name(),
				loc.file_name(),
				loc.line(),
				stacktrace
			);
		}
	};

	class ComError : std::runtime_error
	{
	public:
		ComError(
			Win32::HRESULT hr,
			std::string_view msg,
			const std::source_location location = std::source_location::current(),
			const std::stacktrace& trace = std::stacktrace::current()
		) : m_hresult(hr), std::runtime_error(Format(hr, msg, location, trace))
		{
		}

		auto GetHResult(this const ComError& self) noexcept -> Win32::HRESULT
		{
			return self.m_hresult;
		}

	private:
		static auto Format(
			Win32::HRESULT hr,
			std::string_view message,
			const std::source_location& loc,
			const std::stacktrace& stacktrace
		) -> std::string
		{
			return std::format(
				"HRESULT error (code {}): {}\n"
				"  at {} ({}:{})\n"
				"Stacktrace:\n{}",
				hr,
				TranslateErrorCode(hr),
				loc.function_name(),
				loc.file_name(),
				loc.line(),
				stacktrace
			);
		}

		Win32::HRESULT m_hresult;
	};
}
