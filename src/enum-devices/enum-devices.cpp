#include <iostream>
#include <vector>
#include "enum-devices.h"
#include <string>
#include <iostream>

using std::vector;
using std::string;
using std::wcout;

IDXGIFactory4* mdxgiFactory;
DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		wcout << text;
	}
}

void LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		wcout << text;

		LogOutputDisplayModes(output, mBackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void LogAdapters() 
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND) 
	{ 
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);
		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";
		wcout << text;
		adapterList.push_back(adapter);
		++i;
	} 
	for (size_t i = 0; i < adapterList.size(); ++i) 
	{ 
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	} 
}

int main()
{
	CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory));
	LogAdapters();
}
