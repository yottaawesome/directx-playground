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

		ComPtr<ID3D12Device> device = CreateAD3D12Device();
		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;
		ComPtr<ID3D12Fence> fence = CreateFence(device.Get(), mRtvDescriptorSize, mDsvDescriptorSize, mCbvSrvUavDescriptorSize);
		if (!AssertMSAAQualitySupport(device.Get()))
		{
			std::cout << "Unable to find acceptable MSAA quality level!" << std::endl;
			return 1;
		}
		CommandObjects commandObjects = { 0 };
		CreateCommandObjects(device.Get(), commandObjects);
		ComPtr<ID3D12CommandQueue> mCommandQueue = commandObjects.mCommandQueue;
		ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc = commandObjects.mDirectCmdListAlloc;
		ComPtr<ID3D12GraphicsCommandList> mCommandList = commandObjects.mCommandList;

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
