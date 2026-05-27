export module dx3d:error.functions;
import std;
import :win32;

export namespace dx3d
{
	constexpr auto GetErrorMessage(Win32::HRESULT hr) -> std::string
	{
		char* errorMsg = nullptr;
		Win32::FormatMessageA(
			Win32::FormatMessageFlags::AllocateBuffer | Win32::FormatMessageFlags::FromSystem | Win32::FormatMessageFlags::IgnoreInserts,
			nullptr,
			hr,
			0,
			reinterpret_cast<char*>(&errorMsg),
			0,
			nullptr
		);
		if (not errorMsg)
			return "Unknown error";

		Win32::LocalFree(errorMsg);
		auto message = std::string{ errorMsg };
		while (message.ends_with('\r') or message.ends_with('\n'))
			message.pop_back();
		return message;
	}
}