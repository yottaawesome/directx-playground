#pragma once
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Kernel32.lib")

#include <dxgi1_4.h>
#include <dxgi.h>
#include <d3d12.h>
#include <D3Dcompiler.h>

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif