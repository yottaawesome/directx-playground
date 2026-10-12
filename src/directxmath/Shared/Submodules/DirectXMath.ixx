module;

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

export module DirectXMath;

export
{
	using
		::DirectX::operator*,
		::DirectX::operator*=,
		::DirectX::operator+,
		::DirectX::operator+=,
		::DirectX::operator-,
		::DirectX::operator-=,
		::DirectX::operator/,
		::DirectX::operator/=
		;
}

export namespace DirectX
{
	constexpr auto Pi = DirectX::XM_PI;
	constexpr auto TwoPi = DirectX::XM_2PI;
	constexpr auto PiOver2 = DirectX::XM_PIDIV2;
	constexpr auto PiOver4 = DirectX::XM_PIDIV4;

	using
		::DirectX::BoundingBox, // replaces XNA::AxisAlignedBox
		::DirectX::BoundingFrustum, // replaces XNA::Frustum
		::DirectX::CXMMATRIX,
		::DirectX::CXMVECTOR,
		::DirectX::FXMMATRIX,
		::DirectX::FXMVECTOR,
		::DirectX::XMFLOAT2,
		::DirectX::XMFLOAT3,
		::DirectX::XMFLOAT4,
		::DirectX::XMFLOAT4X4,
		::DirectX::XMMATRIX,
		::DirectX::XMVECTOR,
		::DirectX::XMVECTORF32,

		// Misc
		::DirectX::XMConvertToDegrees,
		::DirectX::XMConvertToRadians,
		::DirectX::XMConvertVectorFloatToInt,
		::DirectX::XMConvertVectorIntToFloat,
		::DirectX::XMLoadFloat,
		::DirectX::XMLoadFloat2,
		::DirectX::XMLoadFloat3,
		::DirectX::XMLoadFloat4,
		::DirectX::XMLoadFloat4x4,
		::DirectX::XMStoreFloat,
		::DirectX::XMStoreFloat2,
		::DirectX::XMStoreFloat3,
		::DirectX::XMStoreFloat4,
		::DirectX::XMStoreFloat4x4,

		// Plane
		::DirectX::XMPlaneDot,
		::DirectX::XMPlaneDotCoord,
		::DirectX::XMPlaneDotNormal,
		::DirectX::XMPlaneEqual,
		::DirectX::XMPlaneFromPointNormal,
		::DirectX::XMPlaneFromPoints,
		::DirectX::XMPlaneIntersectLine,
		::DirectX::XMPlaneIntersectPlane,
		::DirectX::XMPlaneIsInfinite,
		::DirectX::XMPlaneIsNaN,
		::DirectX::XMPlaneNearEqual,
		::DirectX::XMPlaneNormalize,
		::DirectX::XMPlaneNormalizeEst,
		::DirectX::XMPlaneNotEqual,
		::DirectX::XMPlaneTransform,
		::DirectX::XMPlaneTransformStream,

		// Quaternion
		::DirectX::XMQuaternionIdentity,
		::DirectX::XMQuaternionInverse,
		::DirectX::XMQuaternionMultiply,
		::DirectX::XMQuaternionNormalize,
		::DirectX::XMQuaternionRotationAxis,
		::DirectX::XMQuaternionRotationMatrix,
		::DirectX::XMQuaternionSlerp,
		::DirectX::XMQuaternionSlerpV,

		// XMMatrix
		::DirectX::XMMatrixAffineTransformation,
		::DirectX::XMMatrixDecompose,
		::DirectX::XMMatrixDeterminant,
		::DirectX::XMMatrixIdentity,
		::DirectX::XMMatrixInverse,
		::DirectX::XMMatrixLookAtLH,
		::DirectX::XMMatrixLookToLH,
		::DirectX::XMMatrixMultiply,
		::DirectX::XMMatrixOrthographicOffCenterLH,
		::DirectX::XMMatrixPerspectiveFovLH,
		::DirectX::XMMatrixReflect,
		::DirectX::XMMatrixRotationAxis,
		::DirectX::XMMatrixRotationQuaternion,
		::DirectX::XMMatrixRotationRollPitchYaw,
		::DirectX::XMMatrixRotationRollPitchYawFromVector,
		::DirectX::XMMatrixRotationX,
		::DirectX::XMMatrixRotationY,
		::DirectX::XMMatrixRotationZ,
		::DirectX::XMMatrixScaling,
		::DirectX::XMMatrixScalingFromVector,
		::DirectX::XMMatrixSet,
		::DirectX::XMMatrixShadow,
		::DirectX::XMMatrixTranslation,
		::DirectX::XMMatrixTranslationFromVector,
		::DirectX::XMMatrixTranspose,

		// XMVector
		::DirectX::XMVectorAdd,
		::DirectX::XMVectorAndCInt,
		::DirectX::XMVectorAndInt,
		::DirectX::XMVectorDivide,
		::DirectX::XMVectorEqual,
		::DirectX::XMVectorFalseInt,
		::DirectX::XMVectorGetW,
		::DirectX::XMVectorGetX,
		::DirectX::XMVectorGetY,
		::DirectX::XMVectorGetZ,
		::DirectX::XMVectorGreater,
		::DirectX::XMVectorGreaterOrEqual,
		::DirectX::XMVectorInBounds,
		::DirectX::XMVectorLerp,
		::DirectX::XMVectorLess,
		::DirectX::XMVectorLessOrEqual,
		::DirectX::XMVectorMax,
		::DirectX::XMVectorMin,
		::DirectX::XMVectorMultiplyAdd,
		::DirectX::XMVectorOrInt,
		::DirectX::XMVectorReciprocal,
		::DirectX::XMVectorReplicate,
		::DirectX::XMVectorReplicatePtr,
		::DirectX::XMVectorScale,
		::DirectX::XMVectorSelect,
		::DirectX::XMVectorSet,
		::DirectX::XMVectorSetW,
		::DirectX::XMVectorSetX,
		::DirectX::XMVectorSetY,
		::DirectX::XMVectorSetZ,
		::DirectX::XMVectorSplatEpsilon,
		::DirectX::XMVectorSplatW,
		::DirectX::XMVectorSqrt,
		::DirectX::XMVectorSubtract,
		::DirectX::XMVectorSwizzle,
		::DirectX::XMVectorTrueInt,
		::DirectX::XMVectorZero,

		// XMVector3
		::DirectX::XMVector3AngleBetweenNormals,
		::DirectX::XMVector3AngleBetweenNormalsEst,
		::DirectX::XMVector3AngleBetweenVectors,
		::DirectX::XMVector3ClampLength,
		::DirectX::XMVector3ClampLengthV,
		::DirectX::XMVector3ComponentsFromNormal,
		::DirectX::XMVector3Cross,
		::DirectX::XMVector3Dot,
		::DirectX::XMVector3Equal,
		::DirectX::XMVector3EqualInt,
		::DirectX::XMVector3EqualIntR,
		::DirectX::XMVector3EqualR,
		::DirectX::XMVector3Greater,
		::DirectX::XMVector3GreaterOrEqual,
		::DirectX::XMVector3GreaterOrEqualR,
		::DirectX::XMVector3GreaterR,
		::DirectX::XMVector3InBounds,
		::DirectX::XMVector3InverseRotate,
		::DirectX::XMVector3IsInfinite,
		::DirectX::XMVector3IsNaN,
		::DirectX::XMVector3Length,
		::DirectX::XMVector3LengthEst,
		::DirectX::XMVector3LengthSq,
		::DirectX::XMVector3Less,
		::DirectX::XMVector3LessOrEqual,
		::DirectX::XMVector3LinePointDistance,
		::DirectX::XMVector3NearEqual,
		::DirectX::XMVector3Normalize,
		::DirectX::XMVector3NormalizeEst,
		::DirectX::XMVector3NotEqual,
		::DirectX::XMVector3NotEqualInt,
		::DirectX::XMVector3Orthogonal,
		::DirectX::XMVector3Project,
		::DirectX::XMVector3ProjectStream,
		::DirectX::XMVector3ReciprocalLength,
		::DirectX::XMVector3ReciprocalLengthEst,
		::DirectX::XMVector3Reflect,
		::DirectX::XMVector3Refract,
		::DirectX::XMVector3RefractV,
		::DirectX::XMVector3Rotate,
		::DirectX::XMVector3Transform,
		::DirectX::XMVector3TransformCoord,
		::DirectX::XMVector3TransformCoordStream,
		::DirectX::XMVector3TransformNormal,
		::DirectX::XMVector3TransformNormalStream,
		::DirectX::XMVector3TransformStream,
		::DirectX::XMVector3Unproject,
		::DirectX::XMVector3UnprojectStream,

		// XMVector4
		::DirectX::XMVector4AngleBetweenNormals,
		::DirectX::XMVector4AngleBetweenNormalsEst,
		::DirectX::XMVector4AngleBetweenVectors,
		::DirectX::XMVector4ClampLength,
		::DirectX::XMVector4ClampLengthV,
		::DirectX::XMVector4Cross,
		::DirectX::XMVector4Dot,
		::DirectX::XMVector4Equal,
		::DirectX::XMVector4EqualInt,
		::DirectX::XMVector4EqualIntR,
		::DirectX::XMVector4EqualR,
		::DirectX::XMVector4Greater,
		::DirectX::XMVector4GreaterOrEqual,
		::DirectX::XMVector4GreaterOrEqualR,
		::DirectX::XMVector4GreaterR,
		::DirectX::XMVector4InBounds,
		::DirectX::XMVector4IsInfinite,
		::DirectX::XMVector4IsNaN,
		::DirectX::XMVector4Length,
		::DirectX::XMVector4LengthEst,
		::DirectX::XMVector4LengthSq,
		::DirectX::XMVector4Less,
		::DirectX::XMVector4LessOrEqual,
		::DirectX::XMVector4NearEqual,
		::DirectX::XMVector4Normalize,
		::DirectX::XMVector4NormalizeEst,
		::DirectX::XMVector4NotEqual,
		::DirectX::XMVector4NotEqualInt,
		::DirectX::XMVector4Orthogonal,
		::DirectX::XMVector4ReciprocalLength,
		::DirectX::XMVector4ReciprocalLengthEst,
		::DirectX::XMVector4Reflect,
		::DirectX::XMVector4Refract,
		::DirectX::XMVector4RefractV,
		::DirectX::XMVector4Transform,
		::DirectX::XMVector4TransformStream
		;

	namespace PackedVector
	{
		using
			::DirectX::PackedVector::HALF,
			::DirectX::PackedVector::XMCOLOR,
			::DirectX::PackedVector::XMConvertFloatToHalf,
			::DirectX::PackedVector::XMHALF4,
			::DirectX::PackedVector::XMStoreColor
			;
	}

	namespace Colors
	{
		using
			::DirectX::Colors::Black,
			::DirectX::Colors::Blue,
			::DirectX::Colors::Cyan,
			::DirectX::Colors::Green,
			::DirectX::Colors::LightCoral,
			::DirectX::Colors::LightGoldenrodYellow,
			::DirectX::Colors::LightGreen,
			::DirectX::Colors::LightSteelBlue,
			::DirectX::Colors::Magenta,
			::DirectX::Colors::Red,
			::DirectX::Colors::Silver,
			::DirectX::Colors::White,
			::DirectX::Colors::Yellow
			;
	}
}
