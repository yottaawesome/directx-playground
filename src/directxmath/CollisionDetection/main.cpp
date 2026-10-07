import std;
import Shared;

struct Triangle
{
	DirectX::XMFLOAT3 V0{0, 0, 0};
	DirectX::XMFLOAT3 V1{0, 0, 0};
	DirectX::XMFLOAT3 V2{0, 0, 0};
};

struct Sphere
{
	DirectX::XMFLOAT3 Center{ 0, 0, 0 };
	float Radius{ 0 };
};

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
}