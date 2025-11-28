module;

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
		::IID_PPV_ARGS_Helper
		;
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
