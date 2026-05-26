module;

#include <Windows.h>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <d3dcompiler.h>

export module featurelevels;

export namespace Win32
{
	using 
		::HRESULT,
		::ID3D12Device,
		::D3D_FEATURE_LEVEL,
		::D3D12_FEATURE,
		::D3D12_FEATURE_DATA_FEATURE_LEVELS,
		::D3D12CreateDevice
		;

	constexpr auto HrFailed(HRESULT hr) noexcept -> bool { return FAILED(hr); }
}
