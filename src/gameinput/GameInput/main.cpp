#pragma comment(lib, "GameInput.lib")
// Ran into a modules bug: https://developercommunity.visualstudio.com/t/MSVC-fails-to-correctly-export-COM-GUID/11160938
// Works fine if this is included.
// #include <GameInput.h>

import std;
import WindowsPlatform;

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
    auto gameInput = Microsoft::WRL::ComPtr<GameInput::IGameInput>();

    auto result = GameInput::CreateGameInput();
    if (not result)
    {
        Win32::MessageBoxW(nullptr, L"GameInputCreate failed.", L"Initialisation error", Win32::MB::OK | Win32::MB::ICONERROR);
        return 1;
    }

    return 0;
}
