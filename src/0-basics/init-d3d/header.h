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
bool AssertMSAAQualitySupport(ID3D12Device* d3d12Device);
void CreateCommandObjects(ID3D12Device* d3d12Device, CommandObjects& commandObjects);