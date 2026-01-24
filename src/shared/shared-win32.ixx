module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_5.h>
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
	template<auto VValue>
	struct Win32Constant
	{
		operator decltype(VValue)(this auto&&) noexcept
			requires (not std::invocable<decltype(VValue)>)
		{
			return VValue;
		}

		operator decltype(VValue)(this auto&&) noexcept
			requires std::invocable<decltype(VValue)>
		{
			return VValue();
		}
	};

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
		::WNDCLASSEXW,
		::HBRUSH,
		::MSG,
		::LPSTR,
		::GUID,
		::HRESULT,
		::PWSTR,
		::FormatMessageA,
		::LocalFree,
		::PostQuitMessage,
		::PeekMessageW,
		::GetMessageW,
		::TranslateMessage,
		::DispatchMessageW,
		::LoadCursorW,
		::LoadIconW,
		::GetStockObject,
		::ShowWindow,
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

	constexpr Win32Constant<IDI_APPLICATION> IdiApplication;
	constexpr Win32Constant<IDC_ARROW> IdcArrow;

	namespace ShowWindowOptions
	{
		enum
		{
			Hide = SW_HIDE,
			ShowNormal = SW_SHOWNORMAL
		};
	}

	namespace Brushes
	{
		enum
		{
			White = WHITE_BRUSH
		};
	}

	namespace PeekMessageOptions
	{
		enum
		{
			NoRemove = PM_NOREMOVE,
			Remove = PM_REMOVE,
			NoYield = PM_NOYIELD
		};
	}

	namespace FormatMessageOptions
	{
		enum
		{
			AllocateBuffer = FORMAT_MESSAGE_ALLOCATE_BUFFER,
			FromSystem = FORMAT_MESSAGE_FROM_SYSTEM,
			IgnoreInserts = FORMAT_MESSAGE_IGNORE_INSERTS
		};
	}

	constexpr auto MakeHResult(long severity, long facility, long code) noexcept -> HRESULT
	{
		return MAKE_HRESULT(severity, facility, code);
	}

	constexpr auto Facility(HRESULT hr) noexcept -> long
	{
		return HRESULT_FACILITY(hr);
	}

	constexpr auto Code(HRESULT hr) noexcept -> long
	{
		return HRESULT_CODE(hr);
	}

	constexpr auto Severity(HRESULT hr) noexcept -> long
	{
		return HRESULT_SEVERITY(hr);
	}

	constexpr auto HrFailed(HRESULT hr) noexcept -> bool
	{
		return FAILED(hr);
	}

	constexpr auto HrSuccess(HRESULT hr) noexcept -> bool
	{
		return SUCCEEDED(hr);
	}
}

export namespace D3D12
{
	using 
		::D3D12CreateDevice,
		::D3D12_COMMAND_QUEUE_DESC,
		::D3D12_DESCRIPTOR_HEAP_DESC,
		::D3D12_COMMAND_LIST_TYPE,
		::D3D12_COMMAND_QUEUE_FLAGS,
		::ID3D12CommandQueue,
		::ID3D12CommandAllocator,
		::ID3D12GraphicsCommandList,
		::ID3D12SwapChainAssistant,
		::ID3D12CommandList,
		::ID3D12DescriptorHeap,
		::D3D12_DESCRIPTOR_HEAP_TYPE,
		::D3D12_DESCRIPTOR_HEAP_FLAGS,
		::D3D12_FENCE_FLAGS,
		::ID3D12Fence,
		::ID3D12Device,
		::D3D12_FEATURE,
		::D3D_FEATURE_LEVEL,
		::D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS,
		::D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS
		;
}

export namespace DXGI
{
	constexpr auto UsageRenderTargetOutput = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	using 
		::CreateDXGIFactory1,
		::CreateDXGIFactory2,
		::IDXGISwapChain,
		::IDXGIFactory,
		::IDXGIFactory1,
		::IDXGIFactory4,
		::IDXGIAdapter,
		::IDXGIOutput,
		::DXGI_SWAP_CHAIN_DESC,
		::DXGI_ADAPTER_DESC,
		::DXGI_OUTPUT_DESC,
		::DXGI_MODE_DESC,
		::DXGI_FORMAT,
		::DXGI_SWAP_EFFECT,
		::DXGI_SWAP_CHAIN_FLAG
		;

	namespace Error
	{
		enum 
		{
			NotFound = DXGI_ERROR_NOT_FOUND
		};
	}
}
