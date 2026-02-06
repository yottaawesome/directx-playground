export module shared:app.common;
import std;
import :win32;
import :raii;

export namespace App
{
	struct Dimensions
	{
		unsigned Width = 0;
		unsigned Height = 0;
	};

	using HwndUniquePtr = std::unique_ptr<std::remove_pointer_t<Win32::HWND>, Raii::Deleter<Win32::DestroyWindow>>;

	template<Win32::DWORD VMessageId>
	struct Win32Message
	{
		Win32::HWND hwnd = nullptr;
		static constexpr unsigned uMsg = VMessageId;
		Win32::WPARAM wParam = 0;
		Win32::LPARAM lParam = 0;
	};

	template<typename T, typename M>
	concept Handles = requires(T t, M m)
	{
		{ t.OnMessage(m) } -> std::convertible_to<Win32::LRESULT>;
	};

	// Add more message types as needed
	constexpr std::array HandledMessages{
		Win32::Messages::Destroy,
		Win32::Messages::Paint,
		Win32::Messages::KeyUp,
		Win32::Messages::Command,
		Win32::Messages::NonClientDestroy,
		Win32::Messages::Size,
		Win32::Messages::EnterSizeMove,
		Win32::Messages::ExitSizeMove
	};
}
