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
