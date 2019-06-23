#include "header.h"
#include <wrl.h>

using Microsoft::WRL::ComPtr;

ID3D12Device* CreateAD3D12Device()
{
	// Step 1: acquire device
	ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();

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

UINT DetermineMSAAQualitySupport(ID3D12Device* d3d12Device, DXGI_FORMAT mBackBufferFormat, UINT sampleCount)
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = sampleCount;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	d3d12Device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels));

	return msQualityLevels.NumQualityLevels;
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

IDXGISwapChain* CreateSwapChain(IDXGIFactory4* mdxgiFactory, ID3D12CommandQueue* mCommandQueue, HWND mhMainWnd, UINT mClientWidth, UINT mClientHeight, DXGI_FORMAT mBackBufferFormat, UINT SwapChainBufferCount, bool useMsaa, UINT samples, UINT m4xMsaaQuality)
{
	IDXGISwapChain* mSwapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC sd;  
	sd.BufferDesc.Width = mClientWidth;  
	sd.BufferDesc.Height = mClientHeight;  
	sd.BufferDesc.RefreshRate.Numerator = 60;  
	sd.BufferDesc.RefreshRate.Denominator = 1;  
	sd.BufferDesc.Format = mBackBufferFormat;  
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;  
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;  
	sd.SampleDesc.Count = useMsaa ? samples : 1;
	sd.SampleDesc.Quality = useMsaa ? (m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  
	sd.BufferCount = SwapChainBufferCount;  
	sd.OutputWindow = mhMainWnd;  
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;  
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  
	// Note: Swap chain uses queue to perform flush.  
	ThrowIfFailed(mdxgiFactory->CreateSwapChain(mCommandQueue, &sd, &mSwapChain));
	return mSwapChain;
}

void CreateRtvAndDsvDescriptorHeaps(ID3D12Device* md3dDevice, UINT SwapChainBufferCount, ID3D12DescriptorHeap** mRtvHeap, ID3D12DescriptorHeap** mDsvHeap)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap)));
}