#include <iostream>
#include "../shared/shared.h"
#include <wrl.h>

using Microsoft::WRL::ComPtr;

int main()
{
	ComPtr<ID3D12Device> d3d12Device;
	D3D12CreateDevice(
		nullptr, // use default adapter
		D3D_FEATURE_LEVEL_12_1, // https://docs.microsoft.com/en-us/windows/desktop/api/d3dcommon/ne-d3dcommon-d3d_feature_level
		IID_PPV_ARGS(&d3d12Device)
	);

	if (d3d12Device == nullptr)
	{
		std::cout << "Failed to create device\n";
		return 1;
	}

	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels; 
	msQualityLevels.Format = mBackBufferFormat; 
	msQualityLevels.SampleCount = 4; 
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE; 
	msQualityLevels.NumQualityLevels = 0; 

	// https://docs.microsoft.com/en-us/windows/desktop/api/d3d12/nf-d3d12-id3d12device-checkfeaturesupport
	d3d12Device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, // https://docs.microsoft.com/en-us/windows/desktop/api/d3d12/ne-d3d12-d3d12_feature
		&msQualityLevels, // This data structure depends on the D3D12_FEATURE parameter
		sizeof(msQualityLevels));

	std::cout << "Requesting quality levels for:" << std::endl;
	std::cout << "  Back buffer format: " << msQualityLevels.Format << std::endl;
	std::cout << "  Sample count: " << msQualityLevels.SampleCount << std::endl;
	std::cout << "  Flags: " << msQualityLevels.Flags << std::endl;
	std::cout << "Quality levels: " << msQualityLevels.NumQualityLevels << std::endl;

	return 0;
}

