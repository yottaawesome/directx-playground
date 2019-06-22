#include "header.h"
#include <wrl.h>
#include <iostream>
#include <map>

using Microsoft::WRL::ComPtr;

int main(int argc, char* argv[])
{
	// Step 1: acquire device
	ComPtr<ID3D12Device> d3d12Device;
	ThrowIfFailed(
		D3D12CreateDevice(
			nullptr, // use default adapter
			D3D_FEATURE_LEVEL_12_1, // https://docs.microsoft.com/en-us/windows/desktop/api/d3dcommon/ne-d3dcommon-d3d_feature_level
			IID_PPV_ARGS(&d3d12Device)));

	// Step 2: arrange feature levels and presentation
	const int numFeatureLevels = 5;
	D3D_FEATURE_LEVEL featureLevels[numFeatureLevels] =
	{ 
		D3D_FEATURE_LEVEL_12_1,	// Check for 12.1 support
		D3D_FEATURE_LEVEL_12_0,	// Then check for 12 support
		D3D_FEATURE_LEVEL_11_0, // Then check for API 11 support
		D3D_FEATURE_LEVEL_10_0,	// And so on...
		D3D_FEATURE_LEVEL_9_3
	};

	std::map<D3D_FEATURE_LEVEL, std::string> featureLevel = 
	{
		{ D3D_FEATURE_LEVEL_12_1, "D3D12.1"},
		{ D3D_FEATURE_LEVEL_12_0, "D3D12.0"},
		{ D3D_FEATURE_LEVEL_11_0, "D3D11.0"},
		{ D3D_FEATURE_LEVEL_10_0, "D3D10.0"},
		{ D3D_FEATURE_LEVEL_9_3, "D3D9.3"}
	};

	// Step 3: populate request struct
	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo;
	featureLevelsInfo.NumFeatureLevels = numFeatureLevels;
	featureLevelsInfo.pFeatureLevelsRequested = featureLevels;

	// Step 4: request information 
	ThrowIfFailed(d3d12Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelsInfo, sizeof(featureLevelsInfo)));

	std::cout << "Checking support for the following feature levels: " << std::endl;
	for (auto const& key : featureLevels)
		std::cout << "    " << featureLevel[key] << std::endl;

	// Step 5: print information
	std::cout 
		<< std::endl
		<< "Highest feature level supported: " 
		<< featureLevel[featureLevelsInfo.MaxSupportedFeatureLevel] 
		<< std::endl;
}
