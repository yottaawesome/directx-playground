module;

#include <Windows.h>
#include <wrl/client.h>
#include <GameInput.h>

export module WindowsPlatform;
import std;


export namespace Win32
{
	using 
		::HANDLE,
		::HRESULT,
		::HWND,
		::HINSTANCE,
		::LPWSTR,
		::MessageBoxW
		;

	export namespace MB
	{
		constexpr auto OK = MB_OK;
		constexpr auto ICONERROR = MB_ICONERROR;
	}

	constexpr auto HrFailed(HRESULT hr) -> bool
	{
		return hr < 0;
	}
}

export namespace Microsoft::WRL
{
	using
		::Microsoft::WRL::ComPtr
		;
}

export namespace GameInput
{
	// Workaround for https://developercommunity.visualstudio.com/t/MSVC-fails-to-correctly-export-COM-GUID/11160938
	using CreateExpected = std::expected<::GameInput::v3::IGameInput*, ::HRESULT>;
	constexpr auto CreateGameInput() -> CreateExpected;

	using
		::GameInput::v3::IGameInput,
		::GameInput::v3::GameInputCreate
		;
}