#include "header.h"
#include <wrl.h>

using Microsoft::WRL::ComPtr;

ID3D12Device* CreateAD3D12Device()
{
	// Step 1: acquire device
	ID3D12Device* d3d12Device;
	ThrowIfFailed(
		D3D12CreateDevice(
			nullptr, // use default adapter
			D3D_FEATURE_LEVEL_12_1, // https://docs.microsoft.com/en-us/windows/desktop/api/d3dcommon/ne-d3dcommon-d3d_feature_level
			IID_PPV_ARGS(&d3d12Device)));

	return d3d12Device;
}

ID3D12Fence* CreateFence(ID3D12Device* d3d12Device, UINT& mRtvDescriptorSize, UINT& mDsvDescriptorSize, UINT& mCbvSrvUavDescriptorSize)
{
	ID3D12Fence* fence;
	ThrowIfFailed(d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	mRtvDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return fence;
}

bool AssertMSAAQualitySupport(ID3D12Device* d3d12Device)
{
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	d3d12Device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels));

	return msQualityLevels.NumQualityLevels > 0;
}

void CreateCommandObjects(ID3D12Device* d3d12Device, CommandObjects& commandObjects)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandObjects.mCommandQueue)));
	ThrowIfFailed(d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandObjects.mDirectCmdListAlloc)));
	ThrowIfFailed(d3d12Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandObjects.mDirectCmdListAlloc,
		nullptr,
		IID_PPV_ARGS(&commandObjects.mCommandList)));
	commandObjects.mCommandList->Close();
}