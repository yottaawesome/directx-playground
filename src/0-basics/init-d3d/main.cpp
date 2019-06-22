#include "header.h"
#include <iostream>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

int main(int argc, char* argv[])
{
	try
	{
		RegisterWindowClass();
		HWND hWnd = CreateMainWindow();

		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		UINT sampleCount = 4;

		// Create Device
		ComPtr<ID3D12Device> device = CreateAD3D12Device();
		
		// Create Fence
		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;
		ComPtr<ID3D12Fence> fence = CreateFence(device.Get(), mRtvDescriptorSize, mDsvDescriptorSize, mCbvSrvUavDescriptorSize);

		UINT msaaQualityLevels = DetermineMSAAQualitySupport(device.Get(), mBackBufferFormat, sampleCount);

		// Create Command Objects
		CommandObjects commandObjects = { 0 };
		CreateCommandObjects(device.Get(), commandObjects);
		ComPtr<ID3D12CommandQueue> mCommandQueue = commandObjects.mCommandQueue;
		ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc = commandObjects.mDirectCmdListAlloc;
		ComPtr<ID3D12GraphicsCommandList> mCommandList = commandObjects.mCommandList;

		// Create 
		IDXGIFactory4* mdxgiFactory;
		CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory));
		RECT r;
		GetClientRect(hWnd, &r);
		ComPtr<IDXGISwapChain> swapChain = CreateSwapChain(
			mdxgiFactory, 
			mCommandQueue.Get(), 
			hWnd, 
			r.right, 
			r.bottom, 
			mBackBufferFormat, 
			2, 
			false, 
			sampleCount,
			msaaQualityLevels);

		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return (int)msg.wParam;
	}
	catch (DxException& error)
	{
		std::cout << "DxException! " << error.ToString() << std::endl;
		return 1;
	}
	catch (std::runtime_error& error)
	{
		std::cout << "Exception: " << error.what() << std::endl;
		return 1;
	}
}
