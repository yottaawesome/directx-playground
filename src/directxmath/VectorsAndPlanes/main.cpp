import std;
import Shared;

void Test()
{
	auto p_init = DirectX::XMFLOAT3{ 0.5f, 0.5f, 0.0f };
	auto a_init = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	auto b_init = DirectX::XMFLOAT3{ 2.0f, 0.0f, 0.0f };
	auto c_init = DirectX::XMFLOAT3{ 0.0f, 2.0f, 0.0f };

	auto P = DirectX::XMLoadFloat3(&p_init);
	auto A = DirectX::XMLoadFloat3(&a_init);
	auto B = DirectX::XMLoadFloat3(&b_init);
	auto C = DirectX::XMLoadFloat3(&c_init);

	if (auto weights = Math::GetBarycentric3D(P, A, B, C); weights)
		std::println("Weights: wA = {}, wB = {}, wC = {}", weights->A, weights->B, weights->C);
	else 
		std::cout << "Error: The triangle is degenerate.\n";
}

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	Test();
	return 0;
}
