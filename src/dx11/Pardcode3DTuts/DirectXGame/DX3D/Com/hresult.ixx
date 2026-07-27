export module dx3d:com.hresult;
import std;
import :win32;
import :error;

export namespace dx3d
{
	struct HResult
	{
		Win32::HRESULT Code = 0;
		constexpr HResult() noexcept = default;
		constexpr HResult(Win32::HRESULT hr) noexcept : Code(hr) {}
		constexpr auto Succeeded() const noexcept { return Win32::Succeeded(Code); }
		constexpr auto Failed() const noexcept { return Win32::Failed(Code); }
		constexpr operator Win32::HRESULT() const noexcept { return Code; }
		constexpr bool operator==(const HResult& other) const noexcept = default;
		constexpr operator bool() const noexcept { return Succeeded(); }
	};

	auto HResultToString(Win32::HRESULT hr) -> std::string
	{
		constexpr long FacilityWin32 = 7;
		const auto facility = (static_cast<unsigned long>(hr) >> 16) & 0x1FFFul;
		if (facility == FacilityWin32)
			return GetErrorMessage(static_cast<Win32::DWORD>(hr) & 0xFFFFul);
		return GetErrorMessage(static_cast<Win32::DWORD>(hr));
	}
}
