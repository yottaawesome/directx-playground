#pragma once
#pragma comment(lib, "../lib/lib-shared.lib")
#include "../lib-shared/include/shared.h"
#include <wrl.h>

// Windowing function
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void RegisterWindowClass();
HWND CreateMainWindow();

// D3D12
struct CommandObjects
{
	ID3D12CommandQueue* mCommandQueue;
	ID3D12CommandAllocator* mDirectCmdListAlloc;
	ID3D12GraphicsCommandList* mCommandList;
};
ID3D12Device* CreateAD3D12Device();
ID3D12Fence* CreateFence(ID3D12Device* d3d12Device, UINT& mRtvDescriptorSize, UINT& mDsvDescriptorSize, UINT& mCbvSrvUavDescriptorSize);
UINT DetermineMSAAQualitySupport(ID3D12Device* d3d12Device, DXGI_FORMAT mBackBufferFormat, UINT sampleCount);
void CreateCommandObjects(ID3D12Device* d3d12Device, CommandObjects& commandObjects);
IDXGISwapChain* CreateSwapChain(IDXGIFactory4* mdxgiFactory, ID3D12CommandQueue* mCommandQueue, HWND mhMainWnd, UINT mClientWidth, UINT mClientHeight, DXGI_FORMAT mBackBufferFormat, UINT SwapChainBufferCount, bool useMsaa, UINT samples, UINT m4xMsaaQuality);
void CreateRtvAndDsvDescriptorHeaps(ID3D12Device* md3dDevice, UINT SwapChainBufferCount, ID3D12DescriptorHeap** mRtvHeap, ID3D12DescriptorHeap** mDsvHeap);
void CreateRenderTargetView(ID3D12Device* md3dDevice, IDXGISwapChain* mSwapChain, ID3D12DescriptorHeap* mRtvHeap, UINT& mRtvDescriptorSize, const UINT SwapChainBufferCount, Microsoft::WRL::ComPtr<ID3D12Resource>* mSwapChainBuffer);
void FlushCommandQueue(UINT64& mCurrentFence, ID3D12CommandQueue* mCommandQueue, ID3D12Fence* mFence);
void CreateDepthAndStencilBuffer(
	ID3D12Device* md3dDevice,
	IDXGISwapChain* mSwapChain,
	ID3D12DescriptorHeap* mRtvHeap,
	ID3D12DescriptorHeap* mDsvHeap,
	const UINT SwapChainBufferCount,
	UINT mClientWidth,
	UINT mClientHeight,
	DXGI_FORMAT mBackBufferFormat,
	Microsoft::WRL::ComPtr<ID3D12Resource>* mSwapChainBuffer,
	UINT mRtvDescriptorSize,
	bool m4xMsaaState,
	UINT samples,
	UINT m4xMsaaQuality,
	DXGI_FORMAT mDepthStencilFormat,
	Microsoft::WRL::ComPtr<ID3D12Resource>& mDepthStencilBuffer,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList,
	ID3D12CommandQueue* mCommandQueue,
	UINT64& mCurrentFence,
	ID3D12Fence* mFence,
	D3D12_VIEWPORT& mScreenViewport,
	D3D12_RECT& mScissorRect,
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc
);
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
);