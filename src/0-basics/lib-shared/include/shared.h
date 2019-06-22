#pragma once
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Kernel32.lib")

#include <dxgi1_4.h>
#include <dxgi.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <string>
#include <comdef.h>

class DxException
{
	public:
		DxException() = default;
		DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

		std::wstring ToString()const;

		HRESULT ErrorCode = S_OK;
		std::wstring FunctionName;
		std::wstring Filename;
		int LineNumber = -1;
};

void ReleaseCom(IUnknown* pUnk);
std::wstring AnsiToWString(const std::string& str);

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
	{                                                                     \
		HRESULT hr__ = (x);                                               \
		if(FAILED(hr__)) { std::wstring wfn = AnsiToWString(__FILE__); throw DxException(hr__, L#x, wfn, __LINE__); } \
	}
#endif