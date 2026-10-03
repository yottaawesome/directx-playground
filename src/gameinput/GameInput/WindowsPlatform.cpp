module;

#include <GameInput.h>

module WindowsPlatform;
import std;

namespace GameInput
{
	constexpr auto CreateGameInput() -> CreateExpected
	{
		auto gameInput = static_cast<::GameInput::v3::IGameInput*>(nullptr);
		if (auto hr = GameInputCreate(&gameInput); Win32::HrFailed(hr))
			return std::unexpected(hr);
		return gameInput;
	}
}
