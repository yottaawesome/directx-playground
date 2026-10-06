import std;
import Shared;

//
// Print utility functions;
namespace
{
	void Print(const DirectX::XMVECTOR& v)
	{
		auto f = DirectX::XMFLOAT3{};
		DirectX::XMStoreFloat3(&f, v);
		std::cout << "Vector: (" << f.x << ", " << f.y << ", " << f.z << ")\n";
	}

	void Print(const DirectX::XMFLOAT3& f)
	{
		std::cout << "XMFLOAT3: (" << f.x << ", " << f.y << ", " << f.z << ")\n";
	}

	void Print(const DirectX::XMFLOAT4& f)
	{
		std::cout << "XMFLOAT4: (" << f.x << ", " << f.y << ", " << f.z << ", " << f.w << ")\n";
	}

	void Print(const DirectX::XMFLOAT4X4& m)
	{
		std::cout << "XMFLOAT4X4:\n";
		std::cout << "[" << m._11 << ", " << m._12 << ", " << m._13 << ", " << m._14 << "]\n";
		std::cout << "[" << m._21 << ", " << m._22 << ", " << m._23 << ", " << m._24 << "]\n";
		std::cout << "[" << m._31 << ", " << m._32 << ", " << m._33 << ", " << m._34 << "]\n";
		std::cout << "[" << m._41 << ", " << m._42 << ", " << m._43 << ", " << m._44 << "]\n";
	}

	void Print(const DirectX::XMMATRIX& m)
	{
		auto f = DirectX::XMFLOAT4X4{};
		DirectX::XMStoreFloat4x4(&f, m);
		Print(f);
	}
}

/*
* XMFLOAT3 is the DX math equivalent of a 3D vector with 3 float components (x, y, z).
* It's a simple structure used for representing points or vectors (directions) in 3D space.
* XMVECTOR is a type that represents a vector in SIMD (Single Instruction, Multiple Data) 
* format, which is optimized for performance on modern CPUs. In general, you use XMFLOAT3 
* for storage and XMVECTOR for calculations.
*/
void LoadAndStoreFloat3()
{
	std::println("==== Load and Store Float3 Operations ====");

	auto v1 = DirectX::XMFLOAT3{ 1.0f, 2.0f, 3.0f };
	// Prior to calculations, you would load the XMFLOAT3 into an XMVECTOR using XMLoadFloat3.
	auto v2 = DirectX::XMVECTOR{ DirectX::XMLoadFloat3(&v1) };
	auto v3 = DirectX::XMVECTOR{ DirectX::XMVectorSet(4.0f, 5.0f, 6.0f, 0.0f) };
	// Perform some calculations with the XMVECTORs. For example, let's add them together.
	v2 += v3;

	// After calculations, you can store the result back into an XMFLOAT3 using XMStoreFloat3.
	auto v4 = DirectX::XMFLOAT3{};
	DirectX::XMStoreFloat3(&v4, v2);
	std::println("===========================");
}

void LoadAndStoreFloat4()
{
	std::println("==== Load and Store Float4 Operations ====");

	auto v1 = DirectX::XMFLOAT4{ 1.0f, 2.0f, 3.0f, 4.0f };
	// Prior to calculations, you would load the XMFLOAT4 into an XMVECTOR using XMLoadFloat4.
	auto v2 = DirectX::XMVECTOR{ DirectX::XMLoadFloat4(&v1) };
	auto v3 = DirectX::XMVECTOR{ DirectX::XMVectorSet(5.0f, 6.0f, 7.0f, 8.0f) };
	// Perform some calculations with the XMVECTORs. For example, let's add them together.
	v2 += v3;
	// After calculations, you can store the result back into an XMFLOAT4 using XMStoreFloat4.
	auto v4 = DirectX::XMFLOAT4{};
	DirectX::XMStoreFloat4(&v4, v2);

	std::println("===========================");
}

void RotateVector()
{
	std::println("==== Rotation Operations ====");

	// Create a vector to rotate.
	auto v = DirectX::XMVECTOR{ DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) };
	// Define the angle of rotation in degrees and convert it to radians.
	auto angleInDegrees = 90.0f;
	auto angleInRadians = DirectX::XMConvertToRadians(angleInDegrees);
	// Create a rotation matrix around the Z-axis.
	auto rotationMatrix = DirectX::XMMATRIX{ DirectX::XMMatrixRotationZ(angleInRadians) };
	// Rotate the vector using the rotation matrix.
	auto rotatedVector = DirectX::XMVector3Transform(v, rotationMatrix);
	// Store the result in an XMFLOAT3 for later use or inspection.
	auto result = DirectX::XMFLOAT3{};
	DirectX::XMStoreFloat3(&result, rotatedVector);
	Print(result);

	std::println("===========================");
}

void ReflectVector()
{
	std::println("==== Reflection Operations ====");

	// Create a vector to reflect.
	auto original = DirectX::XMFLOAT4{ 1.0f, -1.0f, 0.0f, 0.0f };
	// Load the XMFLOAT4 into an XMVECTOR for calculations.
	auto v = DirectX::XMVECTOR{ DirectX::XMLoadFloat4(&original) };
	// Define a normal for the reflection plane (e.g., reflecting off the X-axis).
	auto normal = DirectX::XMVECTOR{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
	// Reflect the vector using the normal.
	auto reflectedVector = DirectX::XMVector4Reflect(v, normal);
	// Store the result in an XMFLOAT4 for later use or inspection.
	auto result = DirectX::XMFLOAT4{};
	DirectX::XMStoreFloat4(&result, reflectedVector);

	std::print("Original Vector: ");
	Print(original);
	std::print("Reflected Vector: ");
	Print(result);

	std::println("===========================");
}

void MatrixOperations()
{
	std::println("==== Matrix Operations ====");

	// DirectXMath uses radians for angle measurements, so we need to convert degrees to radians.
	auto angleInDegrees = 45.0f;
	auto angleInRadians = DirectX::XMConvertToRadians(angleInDegrees);

	// Create a SIMD-friendly XMMATRIX for the identity matrix.
	auto matrixToTransform = DirectX::XMMATRIX{ DirectX::XMMatrixIdentity() };

	// Create a SIMD-friendly XMMATRIX for rotation around the X-axis and a translation matrix.
	auto m1 = DirectX::XMMATRIX{ DirectX::XMMatrixRotationX(angleInRadians) };
	auto m2 = DirectX::XMMATRIX{ DirectX::XMMatrixTranslation(10.0f, 0.0f, 5.0f) };
	// Combine the two matrices by multiplying them.
	auto m3 = DirectX::XMMATRIX{ DirectX::XMMatrixMultiply(m1, m2) };

	// Apply the combined transformation to the original matrix.
	// Since the original matrix is the identity matrix, this 
	// operation effectively sets it to the combined transformation.
	matrixToTransform *= m3;

	// Store the result in an XMFLOAT4X4 for later use or inspection.
	auto result = DirectX::XMFLOAT4X4{};
	DirectX::XMStoreFloat4x4(&result, matrixToTransform);

	Print(result);

	std::println("===========================");
}

auto main() -> int
{
	RotateVector();
	ReflectVector();
}
