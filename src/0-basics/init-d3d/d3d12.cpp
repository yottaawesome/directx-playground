#include "header.h"
#include <wrl.h>
#include <DirectXColors.h>

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

IDXGISwapChain* CreateSwapChain(
	IDXGIFactory4* mdxgiFactory, 
	ID3D12CommandQueue* mCommandQueue, 
	HWND mhMainWnd, 
	UINT mClientWidth, 
	UINT mClientHeight, 
	DXGI_FORMAT mBackBufferFormat, 
	UINT SwapChainBufferCount, 
	bool useMsaa, 
	UINT samples, 
	UINT m4xMsaaQuality
)
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

void CreateRenderTargetView(ID3D12Device* md3dDevice, IDXGISwapChain* mSwapChain, ID3D12DescriptorHeap* mRtvHeap, UINT& mRtvDescriptorSize, const UINT SwapChainBufferCount, ComPtr<ID3D12Resource>* mSwapChainBuffer)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++) 
	{  
		// Get the ith buffer in the swap chain.  
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));  
		// Create an RTV to it.  
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);  
		// Next entry in heap.  
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}
}

void FlushCommandQueue(UINT64& mCurrentFence, ID3D12CommandQueue* mCommandQueue, ID3D12Fence* mFence)
{
	// Advance the fence value to mark commands up to this fence point.
	mCurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(mCommandQueue->Signal(mFence, mCurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void CreateDepthAndStencilBuffer(
	ID3D12Device* md3dDevice,
	IDXGISwapChain* mSwapChain, 
	ID3D12DescriptorHeap* mRtvHeap, 
	ID3D12DescriptorHeap* mDsvHeap,
	const UINT SwapChainBufferCount, 
	UINT mClientWidth, 
	UINT mClientHeight, 
	DXGI_FORMAT mBackBufferFormat, 
	ComPtr<ID3D12Resource>* mSwapChainBuffer,
	UINT mRtvDescriptorSize,
	bool m4xMsaaState,
	UINT samples, 
	UINT m4xMsaaQuality,
	DXGI_FORMAT mDepthStencilFormat,
	ComPtr<ID3D12Resource>& mDepthStencilBuffer,
	ComPtr<ID3D12GraphicsCommandList>& mCommandList,
	ID3D12CommandQueue* mCommandQueue,
	UINT64& mCurrentFence,
	ID3D12Fence* mFence,
	D3D12_VIEWPORT& mScreenViewport,
	D3D12_RECT& mScissorRect,
	ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc
)
{
	FlushCommandQueue(mCurrentFence, mCommandQueue, mFence);
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Release the previous resources we will be recreating.
	for (UINT i = 0; i < SwapChainBufferCount; ++i)
		mSwapChainBuffer[i].Reset();
	mDepthStencilBuffer.Reset();

	// Resize the swap chain.
	ThrowIfFailed(
		mSwapChain->ResizeBuffers(
			SwapChainBufferCount,
			mClientWidth, 
			mClientHeight,
			mBackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		)
	);

	UINT mCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(
		md3dDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())
		)
	);

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	md3dDevice->CreateDepthStencilView(
		mDepthStencilBuffer.Get(), 
		&dsvDesc, 
		mDsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	// Transition the resource from its initial state to be used as a depth buffer.
	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
		mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	mCommandList->ResourceBarrier(
		1, 
		&transition);

	// Execute the resize commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue(mCurrentFence, mCommandQueue, mFence);

	// Update the viewport transform to cover the client area.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, (LONG)mClientWidth, (LONG)mClientHeight };
}

void Draw(
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList,
	D3D12_VIEWPORT& mScreenViewport,
	ID3D12DescriptorHeap* mDsvHeap,
	D3D12_RECT& mScissorRect,
	const UINT SwapChainBufferCount,
	ID3D12CommandQueue* mCommandQueue,
	UINT64& mCurrentFence,
	UINT mRtvDescriptorSize,
	ID3D12Fence* mFence,
	Microsoft::WRL::ComPtr<ID3D12Resource>* mSwapChainBuffer,
	ID3D12DescriptorHeap* mRtvHeap,
	IDXGISwapChain* mSwapChain,
	UINT& mCurrBackBuffer
)
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Indicate a state transition on the resource usage.
	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		mSwapChainBuffer[mCurrBackBuffer].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	mCommandList->ResourceBarrier(
		1,
		&transition
	);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
			mCurrBackBuffer,
			mRtvDescriptorSize
		), 
		DirectX::Colors::LightSteelBlue,
		0,
		nullptr
	);
	mCommandList->ClearDepthStencilView(
		mDsvHeap->GetCPUDescriptorHandleForHeapStart(), 
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
		1.0f, 
		0, 
		0, 
		nullptr
	);

	// Specify the buffers we are going to render to.
	auto cpuDescriptorHandle = mDsvHeap->GetCPUDescriptorHandleForHeapStart();
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize
	);
	mCommandList->OMSetRenderTargets(
		1,
		&handle,
		true,
		&cpuDescriptorHandle
	);

	// Indicate a state transition on the resource usage.
	auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(
		mSwapChainBuffer[mCurrBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	mCommandList->ResourceBarrier(
		1, 
		&transition2
	);

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue(mCurrentFence, mCommandQueue, mFence);
}