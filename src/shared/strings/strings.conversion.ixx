export module shared:strings.conversion;
import std;
import :win32;
import :error;

export namespace Strings
{
	auto ConvertString(std::wstring_view wstr) -> std::string
	{
		if (wstr.empty())
			return {};

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
		// Returns the size in bytes, this differs from MultiByteToWideChar, which returns the size in characters
		const auto sizeInBytes = int{
			Win32::WideCharToMultiByte(
				Win32::CodePages::Utf8,							// CodePage
				Win32::WideCharOptions::NoBestFitChars,			// dwFlags 
				wstr.data(),									// lpWideCharStr
				static_cast<int>(wstr.size()),					// cchWideChar 
				nullptr,										// lpMultiByteStr
				0,												// cbMultiByte
				nullptr,										// lpDefaultChar
				nullptr											// lpUsedDefaultChar
			) };
		if (sizeInBytes == 0)
			throw Error::Win32Error{ Win32::GetLastError(), "WideCharToMultiByte() [1] failed" };

		auto strTo = std::string(sizeInBytes / sizeof(char), '\0');
		const auto status = int{
			Win32::WideCharToMultiByte(
				Win32::CodePages::Utf8,							// CodePage
				Win32::WideCharOptions::NoBestFitChars,			// dwFlags 
				wstr.data(),									// lpWideCharStr
				static_cast<int>(wstr.size()),					// cchWideChar 
				strTo.data(),									// lpMultiByteStr
				static_cast<int>(strTo.size() * sizeof(char)),	// cbMultiByte
				nullptr,										// lpDefaultChar
				nullptr											// lpUsedDefaultChar
			) };
		if (status == 0)
			throw Error::Win32Error{ Win32::GetLastError(), "WideCharToMultiByte() [2] failed" };

		return strTo;
	}

	auto ConvertString(std::string_view str) -> std::wstring
	{
		if (str.empty())
			return {};

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		// Returns the size in characters, this differs from WideCharToMultiByte, which returns the size in bytes
		const auto sizeInCharacters = int{
			Win32::MultiByteToWideChar(
				Win32::CodePages::Utf8,									// CodePage
				0,											// dwFlags
				str.data(),									// lpMultiByteStr
				static_cast<int>(str.size() * sizeof(char)),// cbMultiByte
				nullptr,									// lpWideCharStr
				0											// cchWideChar
			) };
		if (sizeInCharacters == 0)
			throw Error::Win32Error{ Win32::GetLastError(), "MultiByteToWideChar() [1] failed" };

		auto wstrTo = std::wstring(sizeInCharacters, '\0');
		const auto status = int{
			Win32::MultiByteToWideChar(
				Win32::CodePages::Utf8,						// CodePage
				0,											// dwFlags
				str.data(),									// lpMultiByteStr
				static_cast<int>(str.size() * sizeof(char)),	// cbMultiByte
				wstrTo.data(),								// lpWideCharStr
				static_cast<int>(wstrTo.size())				// cchWideChar
			) };
		if (status == 0)
			throw Error::Win32Error{ Win32::GetLastError(), "MultiByteToWideChar() [2] failed" };

		return wstrTo;
	}
}
