module;

#include <Windows.h>
#include <wrl/client.h>
#include <GameInput.h>

export module WindowsPlatform;

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
	using
		::GameInput::v3::IGameInput,
		::GameInput::v3::GameInputCreate
		;
}