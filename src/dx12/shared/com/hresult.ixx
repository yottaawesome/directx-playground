export module shared:com.hresult;
import std;
import :win32;
import :error;

export namespace Com
{
	struct HResult final
	{
		constexpr HResult() noexcept = default;

		constexpr HResult(const Win32::HRESULT hr) noexcept
			: Hr(hr) {}

		constexpr HResult(long severity, long facility, long code) noexcept
		{
			Hr = Win32::MakeHResult(severity, facility, code);
		}

		constexpr operator Win32::HRESULT() const noexcept { return Hr; }

		constexpr auto operator=(const Win32::HRESULT hr) noexcept -> HResult&
		{
			Hr = hr;
			return *this;
		}

		constexpr operator bool() const noexcept { return Succeeded(); }

		constexpr auto operator==(const Win32::HRESULT hr) const noexcept -> bool { return Hr == hr; }

		constexpr auto operator==(const HResult& hr) const noexcept -> bool { return Hr == hr.Hr; }

		// See https://learn.microsoft.com/en-us/windows/win32/com/using-macros-for-error-handling
		constexpr auto Get() const noexcept -> Win32::HRESULT { return Hr; }

		constexpr auto Facility() const noexcept -> long { return Win32::Facility(Hr); }

		constexpr auto Code() const noexcept -> long { return Win32::Code(Hr); }

		constexpr auto Severity() const noexcept -> long { return Win32::Severity(Hr); }

		constexpr auto Succeeded() const noexcept -> bool { return Win32::HrSuccess(Hr); }

		constexpr auto Failed() const noexcept -> bool { return not Succeeded(); }

		void ThrowIfFailed(std::string_view msg, const std::source_location& loc = std::source_location::current()) const
		{
			if (Succeeded())
				return;
			if (not msg.empty())
				throw Error::ComError(Hr, msg.data(), loc);
			throw Error::ComError(Hr, "HRESULT check failed", loc);
		}

		Win32::HRESULT Hr = 0x0;
	};

	inline void CheckHr(const Win32::HRESULT hr, const std::source_location& loc = std::source_location::current())
	{
		if (Win32::HrFailed(hr))
			throw Error::ComError(hr, "Expected success HRESULT", loc);
	}
}
