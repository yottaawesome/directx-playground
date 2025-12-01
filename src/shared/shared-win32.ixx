module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <d3dcommon.h>

export module shared:win32;

export namespace Microsoft::WRL
{
	using 
		::Microsoft::WRL::ComPtr
		;
}

export namespace Win32
{
	using
		::HINSTANCE,
		::HWND,
		::LPWSTR,
		::ATOM,
		::WNDCLASSW,
		::WPARAM,
		::LPARAM,
		::LRESULT,
		::CREATESTRUCT,
		::LONG_PTR,
		::DWORD,
		::PVOID,
		::LPVOID,
		::GetWindowLongPtrW,
		::DestroyWindow,
		::DefWindowProcW,
		::SetWindowLongPtrW,
		::RegisterClassW,
		::RegisterClassExW,
		::CreateWindowExW,
		::IID_PPV_ARGS_Helper,
		::GetModuleHandleW
		;

	namespace Messages
	{
		enum
		{
			Quit = WM_QUIT,
			Close = WM_CLOSE,
			Destroy = WM_DESTROY,
			Size = WM_SIZE,
			LeftButtonDown = WM_LBUTTONDOWN,
			LeftButtonUp = WM_LBUTTONUP,
			NonClientCreate = WM_NCCREATE,
			Paint = WM_PAINT,
			KeyUp = WM_KEYUP,
			Create = WM_CREATE,
			Command = WM_COMMAND,
			CtlColorBtn = WM_CTLCOLORBTN,
			Notify = WM_NOTIFY,
			SetFont = WM_SETFONT,
			ButtonClick = BM_CLICK,
			ButtonClicked = BN_CLICKED,
			DrawItem = WM_DRAWITEM,
			MouseHover = WM_MOUSEHOVER,
			MouseLeave = WM_MOUSELEAVE,
			MouseMove = WM_MOUSEMOVE,
			EraseBackground = WM_ERASEBKGND,
			NonClientDestroy = WM_NCDESTROY
		};
	}

	namespace WindowStyles
	{
		enum : Win32::DWORD
		{
			WsOverlappedWindow = WS_OVERLAPPEDWINDOW,
			WindowEdge = WS_EX_WINDOWEDGE,
			ThickFrame = WS_THICKFRAME,
			WsVisible = WS_VISIBLE
		};
	}

	constexpr auto Gwlp_UserData = GWLP_USERDATA;
	constexpr auto CwUseDefault = CW_USEDEFAULT;
	constexpr auto SpiGetNonClientMetrics = SPI_GETNONCLIENTMETRICS;
	constexpr auto DefaultCharset = DEFAULT_CHARSET;
}

export namespace D3D12
{
	using 
		::D3D12CreateDevice,
		::ID3D12Device,
		::D3D12_FEATURE,
		::D3D_FEATURE_LEVEL,
		::D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS,
		::D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS
		;
}

export namespace DXGI
{
	using 
		::CreateDXGIFactory1,
		::IDXGIFactory4,
		::IDXGIAdapter,
		::IDXGIOutput,
		::DXGI_ADAPTER_DESC,
		::DXGI_OUTPUT_DESC,
		::DXGI_MODE_DESC,
		::DXGI_FORMAT
		;

	namespace Error
	{
		enum 
		{
			NotFound = DXGI_ERROR_NOT_FOUND
		};
	}
}
