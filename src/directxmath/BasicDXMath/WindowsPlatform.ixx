module;

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

export module WindowsPlatform;

export
{
	using
		::DirectX::operator*,
		::DirectX::operator*=,
		::DirectX::operator-,
		::DirectX::operator-=,
		::DirectX::operator+,
		::DirectX::operator+=,
		::DirectX::operator/=,
		::DirectX::operator/
		;
}

export namespace DirectX
{
	constexpr auto Pi = DirectX::XM_PI;
	constexpr auto TwoPi = DirectX::XM_2PI;

	using
		::DirectX::XMFLOAT4,
		::DirectX::XMMATRIX,
		::DirectX::CXMMATRIX,
		::DirectX::FXMMATRIX,
		::DirectX::FXMVECTOR,
		::DirectX::CXMVECTOR,
		::DirectX::XMVECTORF32,
		::DirectX::XMFLOAT3,
		::DirectX::XMFLOAT2,
		::DirectX::XMFLOAT4X4,
		::DirectX::XMVECTOR,
		::DirectX::BoundingBox, // replaces XNA::AxisAlignedBox
		::DirectX::BoundingFrustum, // replaces XNA::Frustum
		::DirectX::XMVectorFalseInt,
		::DirectX::XMVectorAndCInt,
		::DirectX::XMVectorSqrt,
		::DirectX::XMVector4EqualInt,
		::DirectX::XMVectorInBounds,
		::DirectX::XMVectorSplatEpsilon,
		::DirectX::XMVectorGreaterOrEqual,
		::DirectX::XMVector3NearEqual,
		::DirectX::XMVectorLessOrEqual,
		::DirectX::XMVectorSplatW,
		::DirectX::XMVectorAndInt,
		::DirectX::XMVector3Equal,
		::DirectX::XMVectorLess,
		::DirectX::XMVector3LessOrEqual,
		::DirectX::XMVectorReciprocal,
		::DirectX::XMVector3GreaterOrEqual,
		::DirectX::XMVectorGreater,
		::DirectX::XMVectorSelect,
		::DirectX::XMVector3Reflect,
		::DirectX::XMVector4Reflect,
		::DirectX::XMVectorEqual,
		::DirectX::XMVector3EqualInt,
		::DirectX::XMVectorTrueInt,
		::DirectX::XMVectorOrInt,
		::DirectX::XMPlaneFromPoints,
		::DirectX::XMMatrixDecompose,
		::DirectX::XMVector4Normalize,
		::DirectX::XMMatrixTranslationFromVector,
		::DirectX::XMMatrixScalingFromVector,
		::DirectX::XMVector3Transform,
		::DirectX::XMMatrixOrthographicOffCenterLH,
		::DirectX::XMLoadFloat3,
		::DirectX::XMLoadFloat4x4,
		::DirectX::XMStoreFloat,
		::DirectX::XMMatrixAffineTransformation,
		::DirectX::XMMatrixRotationRollPitchYawFromVector,
		::DirectX::XMVectorLerp,
		::DirectX::XMQuaternionRotationAxis,
		::DirectX::XMVector3Length,
		::DirectX::XMQuaternionSlerp,
		::DirectX::XMVectorSet,
		::DirectX::XMMatrixIdentity,
		::DirectX::XMMatrixMultiply,
		::DirectX::XMMatrixLookAtLH,
		::DirectX::XMMatrixSet,
		::DirectX::XMMatrixLookToLH,
		::DirectX::XMLoadFloat4x4,
		::DirectX::XMConvertToRadians,
		::DirectX::XMMatrixPerspectiveFovLH,
		::DirectX::XMMatrixScaling,
		::DirectX::XMVectorSubtract,
		::DirectX::XMMatrixRotationAxis,
		::DirectX::XMMatrixTranslation,
		::DirectX::XMStoreFloat4x4,
		::DirectX::XMStoreFloat3,
		::DirectX::XMLoadFloat4,
		::DirectX::XMPlaneNormalize,
		::DirectX::XMVectorGetX,
		::DirectX::XMMatrixReflect,
		::DirectX::XMVector3Greater,
		::DirectX::XMVectorMultiplyAdd,
		::DirectX::XMVectorReplicate,
		::DirectX::XMVector3Normalize,
		::DirectX::XMQuaternionInverse,
		::DirectX::XMMatrixRotationQuaternion,
		::DirectX::XMVectorScale,
		::DirectX::XMQuaternionIdentity,
		::DirectX::XMQuaternionSlerpV,
		::DirectX::XMVectorSwizzle,
		::DirectX::XMQuaternionNormalize,
		::DirectX::XMMatrixRotationX,
		::DirectX::XMMatrixRotationY,
		::DirectX::XMMatrixRotationZ,
		::DirectX::XMVectorAdd,
		::DirectX::XMQuaternionMultiply,
		::DirectX::XMVector3Dot,
		::DirectX::XMVector3Cross,
		::DirectX::XMVector3LengthSq,
		::DirectX::XMVector3TransformNormal,
		::DirectX::XMVector3Less,
		::DirectX::XMVector3TransformCoord,
		::DirectX::XMVectorMin,
		::DirectX::XMVectorMax,
		::DirectX::XMMatrixShadow,
		::DirectX::XMVectorReplicatePtr,
		::DirectX::XMMatrixRotationRollPitchYaw,
		::DirectX::XMVectorSqrt,
		::DirectX::XMMatrixRotationY,
		::DirectX::XMVectorZero,
		::DirectX::XMVectorSet,
		::DirectX::XMMatrixDeterminant,
		::DirectX::XMMatrixTranspose,
		::DirectX::XMMatrixInverse,
		::DirectX::XMStoreFloat4
		;

	namespace PackedVector
	{
		using
			::DirectX::PackedVector::XMCOLOR,
			::DirectX::PackedVector::XMHALF4,
			::DirectX::PackedVector::HALF,
			::DirectX::PackedVector::XMConvertFloatToHalf,
			::DirectX::PackedVector::XMStoreColor
			;
	}

	namespace Colors
	{
		using
			::DirectX::Colors::White,
			::DirectX::Colors::Black,
			::DirectX::Colors::Red,
			::DirectX::Colors::Green,
			::DirectX::Colors::Blue,
			::DirectX::Colors::Yellow,
			::DirectX::Colors::Cyan,
			::DirectX::Colors::Magenta,
			::DirectX::Colors::LightCoral,
			::DirectX::Colors::LightGoldenrodYellow,
			::DirectX::Colors::LightGreen,
			::DirectX::Colors::Silver,
			::DirectX::Colors::LightSteelBlue
			;
	}
}
