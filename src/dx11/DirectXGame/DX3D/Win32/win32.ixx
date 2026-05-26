module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>

export module dx3d:win32;

export namespace Win32
{
	template<auto VConstant>
	struct Win32Constant
	{
		constexpr auto operator()(this auto&&) noexcept { return VConstant; }
		constexpr operator decltype(VConstant)(this auto&&) noexcept{ return VConstant; }
	};

	using
		::DWORD,
		::HMODULE,
		::HWND,
		::LRESULT,
		::MSG,
		::WNDCLASSEXW,
		::UINT,
		::WPARAM,
		::LPARAM,
		::ATOM,
		::HBRUSH,
		::CREATESTRUCT,
		::LONG_PTR,
		::GUID,
		::HINSTANCE,
		::RECT,
		::HRESULT,
		::COINIT,
		::PAINTSTRUCT,
		::DPI_AWARENESS,
		::IUnknown,
		::HMENU,
		::UINT_PTR,
		::CLSCTX,
		::UINT32,
		::CREATESTRUCTW,
		::AdjustWindowRect,
		::KillTimer,
		::SetTimer,
		::SetWindowTextW,
		::GetMessageW,
		::UpdateWindow,
		::PostMessageW,
		::CoCreateInstance,
		::IsIconic,
		::AreDpiAwarenessContextsEqual,
		::GetAwarenessFromDpiAwarenessContext,
		::SetProcessDpiAwarenessContext,
		::GetThreadDpiAwarenessContext,
		::SetWindowPos,
		::MessageBoxA,
		::MessageBoxW,
		::InvalidateRect,
		::ValidateRect,
		::CoInitializeEx,
		::CoUninitialize,
		::GetClientRect,
		::BeginPaint,
		::EndPaint,
		::WaitMessage,
		::GetWindowLongPtrW,
		::GetDpiForWindow,
		::SetWindowLongPtrW,
		::ShowWindow,
		::LoadIconW,
		::LoadCursorW,
		::GetStockObject,
		::LocalFree,
		::FormatMessageA,
		::PostQuitMessage,
		::DestroyWindow,
		::GetModuleHandleW,
		::DefWindowProcW,
		::RegisterClassExW,
		::RegisterClassW,
		::PeekMessageW,
		::TranslateMessage,
		::DispatchMessageW,
		::CreateWindowExW,
		::GetLastError
		;

	constexpr auto False = FALSE;
	constexpr auto True = TRUE;

	constexpr auto Succeeded(HRESULT hr) noexcept { return SUCCEEDED(hr); }
	constexpr auto Failed(HRESULT hr) noexcept { return FAILED(hr); }

	constexpr auto CwUseDefault = CW_USEDEFAULT;

	namespace Access
	{
		constexpr auto Read = GENERIC_READ;
		constexpr auto Write = GENERIC_WRITE;
		constexpr auto Execute = GENERIC_EXECUTE;
		constexpr auto All = GENERIC_ALL;
	}

	namespace DpiAwarenessContext
	{
		constexpr auto Unaware = Win32Constant<DPI_AWARENESS_CONTEXT_UNAWARE>{};
		constexpr auto SystemAware = Win32Constant<DPI_AWARENESS_CONTEXT_SYSTEM_AWARE>{};
		constexpr auto PerMonitorAware = Win32Constant<DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE>{};
		constexpr auto PerMonitorAwareV2 = Win32Constant<DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2>{};
	}

	namespace SetWindowPosFlags
	{
		enum
		{
			NoSize = SWP_NOSIZE,
			NoMove = SWP_NOMOVE,
			NoZOrder = SWP_NOZORDER,
			ShowWindow = SWP_SHOWWINDOW,
			HideWindow = SWP_HIDEWINDOW,
			NoActivate = SWP_NOACTIVATE,
			DrawFrame = SWP_DRAWFRAME,
			FrameChanged = SWP_FRAMECHANGED,
			NoOwnerZOrder = SWP_NOOWNERZORDER,
			NoSendChanging = SWP_NOSENDCHANGING
		};
	}

	namespace MessageBoxOptions
	{
		enum
		{
			Ok = MB_OK,
			OkCancel = MB_OKCANCEL,
			AbortRetryIgnore = MB_ABORTRETRYIGNORE,
			YesNoCancel = MB_YESNOCANCEL,
			YesNo = MB_YESNO,
			RetryCancel = MB_RETRYCANCEL,
			Critical = MB_ICONERROR,
			Question = MB_ICONQUESTION,
			Exclamation = MB_ICONEXCLAMATION,
			Information = MB_ICONINFORMATION
		};
	}

	namespace Error
	{
		enum : DWORD
		{
			Success = ERROR_SUCCESS,
			InvalidFunction = ERROR_INVALID_FUNCTION,
			FileNotFound = ERROR_FILE_NOT_FOUND,
			PathNotFound = ERROR_PATH_NOT_FOUND,
			AccessDenied = ERROR_ACCESS_DENIED,
			InvalidHandle = ERROR_INVALID_HANDLE,
			NotEnoughMemory = ERROR_NOT_ENOUGH_MEMORY,
			InvalidData = ERROR_INVALID_DATA,
			OutOfMemory = ERROR_OUTOFMEMORY,
			ClassAlreadyExists = ERROR_CLASS_ALREADY_EXISTS
		};
	}

	namespace ClassStyles
	{
		enum
		{
			DblClks = CS_DBLCLKS,
			DropShadow = CS_DROPSHADOW,
			HRedraw = CS_HREDRAW,
			VRedraw = CS_VREDRAW,
			OwnDC = CS_OWNDC,
			ClassDC = CS_CLASSDC,
			ParentDC = CS_PARENTDC,
			Noclose = CS_NOCLOSE,
			Savedefault = CS_SAVEBITS,
			ByteAlignClient = CS_BYTEALIGNCLIENT,
			ByteAlignWindow = CS_BYTEALIGNWINDOW,
			GlobalClass = CS_GLOBALCLASS,
			Ime = CS_IME,
			Dblclks = CS_DBLCLKS
		};
	}

	namespace WindowStyles
	{
		enum
		{
			Overlapped = WS_OVERLAPPED,
			OverlappedWindow = WS_OVERLAPPEDWINDOW,
			Popup = WS_POPUP,
			Child = WS_CHILD,
			Minimize = WS_MINIMIZE,
			Visible = WS_VISIBLE,
			Disabled = WS_DISABLED,
			ClipSiblings = WS_CLIPSIBLINGS,
			ClipChildren = WS_CLIPCHILDREN,
			Maximize = WS_MAXIMIZE,
			Caption = WS_CAPTION,
			Border = WS_BORDER,
			DlgFrame = WS_DLGFRAME,
			VScroll = WS_VSCROLL,
			HScroll = WS_HSCROLL,
			SysMenu = WS_SYSMENU,
			ThickFrame = WS_THICKFRAME,
			Group = WS_GROUP,
			TabStop = WS_TABSTOP,
			MinimizeBox = WS_MINIMIZEBOX,
			MaximizeBox = WS_MAXIMIZEBOX,
		};
	}

	namespace Gwlp
	{
		enum
		{
			UserData = GWLP_USERDATA,
			WndProc = GWLP_WNDPROC,
			HInstance = GWLP_HINSTANCE,
			HwndParent = GWLP_HWNDPARENT,
			Id = GWLP_ID
		};
	}

	namespace ShowWindowFlags
	{
		enum
		{
			Hide = SW_HIDE,
			ShowNormal = SW_SHOWNORMAL,
			ShowMinimized = SW_SHOWMINIMIZED,
			ShowMaximized = SW_SHOWMAXIMIZED,
			Show = SW_SHOW,
			Minimize = SW_MINIMIZE,
			ShowMinNoActive = SW_SHOWMINNOACTIVE,
			ShowNA = SW_SHOWNA,
			Restore = SW_RESTORE,
			ShowDefault = SW_SHOWDEFAULT,
			ForceMinimize = SW_FORCEMINIMIZE
		};
	}

	namespace FormatMessageFlags
	{
		enum
		{
			IgnoreInserts = FORMAT_MESSAGE_IGNORE_INSERTS,
			FromString = FORMAT_MESSAGE_FROM_STRING,
			AllocateBuffer = FORMAT_MESSAGE_ALLOCATE_BUFFER,
			FromSystem = FORMAT_MESSAGE_FROM_SYSTEM,
		};
	}

	namespace Messages
	{
		enum : UINT
		{
			Quit = WM_QUIT,
			NonClientCreate = WM_NCCREATE,
			NonClientDestroy = WM_NCDESTROY,
			Destroy = WM_DESTROY,
			Close = WM_CLOSE,
			Paint = WM_PAINT,
			KeyUp = WM_KEYUP,
			KeyDown = WM_KEYDOWN,
			Command = WM_COMMAND,
			Size = WM_SIZE,
			DisplayChange = WM_DISPLAYCHANGE,
			DpiChanged = WM_DPICHANGED,
			Timer = WM_TIMER
		};
	}

	namespace Keys
	{
		enum : WPARAM
		{
			Escape = VK_ESCAPE,
			Left = VK_LEFT,
			Up = VK_UP,
			Right = VK_RIGHT,
			Down = VK_DOWN,
			Space = VK_SPACE,
		};
	}

	namespace PeekMessageFlags
	{
		enum
		{
			NoRemove = PM_NOREMOVE,
			Remove = PM_REMOVE,
			NoYield = PM_NOYIELD
		};
	}

	constexpr auto IdiApplication = Win32Constant<IDI_APPLICATION>{};
	constexpr auto IdcArrow = Win32Constant<IDC_ARROW>{};

	namespace Brushes
	{
		constexpr auto White = WHITE_BRUSH;
	}



	constexpr auto LoWord(auto v) noexcept -> WORD { return LOWORD(v); }
	constexpr auto HiWord(auto v) noexcept -> WORD { return HIWORD(v); }

	constexpr auto HrFailed(HRESULT hr) noexcept { return FAILED(hr); }
	constexpr auto SOk = S_OK;
	constexpr auto SFalse = S_FALSE;
}
