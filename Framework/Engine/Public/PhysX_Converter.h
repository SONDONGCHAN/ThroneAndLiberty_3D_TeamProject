#pragma once

namespace physx
{
	/* DirectX -> PhysX */
	inline PxVec2 ToPxVec2(const _float2& _vector)
	{
		return PxVec2(_vector.x, _vector.y);
	}

	inline PxVec3 ToPxVec3(const _float3& _vector)
	{
		return PxVec3(_vector.x, _vector.y, _vector.z);
	}

	inline PxVec4 ToPxVec4(const _float4& _vector)
	{
		return PxVec4(_vector.x, _vector.y, _vector.z, _vector.w);
	}

	inline PxQuat ToPxQuat(const _quaternion& _Quaternion)
	{
		return PxQuat(_Quaternion.x, _Quaternion.y, _Quaternion.z, _Quaternion.w);
	}

	inline PxMat33 ToPxMat33(const _float4x4& _Matrix)
	{
		return PxMat33(ToPxVec3(_Matrix.Right()), ToPxVec3(_Matrix.Up()), ToPxVec3(_Matrix.Forward()));
	}

	inline PxMat44 ToPxMat44(const _float4x4& _Matrix)
	{
		return PxMat44(ToPxVec3(_Matrix.Right()), ToPxVec3(_Matrix.Up()), ToPxVec3(_Matrix.Forward()), ToPxVec3(_Matrix.Translation()));
	}

	inline PxTransform ToPxTransform(const _float3& _vector, const _quaternion& _Quaternion)
	{
		return PxTransform(ToPxVec3(_vector), ToPxQuat(_Quaternion));
	}

	inline PxTransform ToPxTransform(const _float4x4& _Matrix)
	{
		_float4x4 QuaternionMatrix = _Matrix;
		QuaternionMatrix.Translation({ 0.0f,0.0f,0.0f });
		_quaternion vQuaternion = XMQuaternionRotationMatrix(QuaternionMatrix);
		return PxTransform(ToPxVec3(_Matrix.Translation()), ToPxQuat(vQuaternion));
	}

	/* PhysX -> DirectX */
	inline _float2 ToFloat2(const PxVec2& _PxVector)
	{
		return _float2(_PxVector.x, _PxVector.y);
	}

	inline _float3 ToFloat3(const PxVec3& _PxVector)
	{
		return _float3(_PxVector.x, _PxVector.y, _PxVector.z);
	}

	inline _float3 ToFloat3(const PxTransform& _PxTransform)
	{
		return ToFloat3(_PxTransform.p);
	}

	inline _float4 ToFloat4(const PxVec4& _PxVector)
	{
		return _float4(_PxVector.x, _PxVector.y, _PxVector.z, _PxVector.w);
	}

	inline _quaternion ToQuat(const PxQuat& _PxQuaternion)
	{
		return _quaternion(_PxQuaternion.x, _PxQuaternion.y, _PxQuaternion.z, _PxQuaternion.w);
	}

	inline _quaternion ToQuat(const PxTransform& _PxTransform)
	{
		return ToQuat(_PxTransform.q);
	}

	inline _float4x4 ToFloat4x4(const PxMat33& _PxMatrix)
	{
		return _float4x4(ToFloat3(_PxMatrix.column0), ToFloat3(_PxMatrix.column1), ToFloat3(_PxMatrix.column2));
	}

	inline _float4x4 ToFloat4x4(const PxMat44& _PxMatrix)
	{
		return _float4x4(ToFloat4(_PxMatrix.column0), ToFloat4(_PxMatrix.column1), ToFloat4(_PxMatrix.column2), ToFloat4(_PxMatrix.column3));
	}

	inline _float4x4 ToFloat4x4(const PxTransform& _PxTransform)
	{
		return ToFloat4x4(PxMat44(_PxTransform));
	}
}