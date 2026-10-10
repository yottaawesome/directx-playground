import std;
import Shared;

/* 
* The dot product of two vectors produces a scalar quantity. That quantity has the following properties:
* * The dot product is commutative: a · b = b · a
* * The dot product is distributive over vector addition: a · (b + c) = a · b + a · c
* * Scalar multiplication is distributive over the dot product: (da) · b = d(a · b) = a · (db)
* * The dot product of a vector with itself is equal to the square of its magnitude: a · a = |a|^2
* * The dot product of two orthogonal vectors is zero: a · b = 0 if a and b are orthogonal (perpendicular, 90 degrees)
* * The dot product is:
		-> greater than zero if the angle between the two vectors is less than 90 degrees (acute);
		-> less than zero if the angle is greater than 90 degrees but less than 180 degrees (obtuse);
		-> equal to zero if the angle is exactly 90 degrees (right angle).
* * The dot product can be used to project one vector onto another: proj_b(a) = (a · b / |b|^2) * b
* * The dot product can be used to find the angle between two vectors: a · b = |a| * |b| * cos(theta), 
	where theta is the angle between a and b.
*/
namespace Vectors::DotProduct
{
	auto Dot3Basic(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) noexcept -> float
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	auto Dot3Vector(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) noexcept -> float
	{
		auto va = DirectX::XMLoadFloat3(&a);
		auto vb = DirectX::XMLoadFloat3(&b);
		auto dot = DirectX::XMVector3Dot(va, vb);
		return DirectX::XMVectorGetX(dot);
	}
}

auto wWinMain(Win32::HINSTANCE, Win32::HINSTANCE, Win32::LPWSTR, int) -> int
{
	return 0;
}
