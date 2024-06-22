#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CPipeLine final
{
public:
	enum ED3DTRANSFORMSTATE {
		D3DTS_VIEW,
		D3DTS_PROJ,
		D3DTS_END
	};

public:
	CPipeLine();
	~CPipeLine() = default;


public:
	void SetTransform(ED3DTRANSFORMSTATE eState, _float4x4 TransformMatrix) {
		XMStoreFloat4x4(&m_PrevTransformMatrix[eState], m_TransformMatrix[eState]);
		XMStoreFloat4x4(&m_TransformMatrix[eState], TransformMatrix);
	}


	void SetShadowProjMat(_float4x4* _ShadowProjMat)
	{
		memcpy(m_ShadowProjMatrix, _ShadowProjMat, sizeof(_float4x4) * 3);
	}

	void SetShadowViewMat(_float4x4* _ShadowViewMat) 
	{
		memcpy(m_ShadowViewMatrix, _ShadowViewMat, sizeof(_float4x4) * 3);
	}


	void SetShadowWorldMat(_float4x4* _ShadowWorldMat)
	{
		memcpy(m_ShadowReverseProjMat, _ShadowWorldMat, sizeof(_float4x4) * 3);
	}

	_float4x4 GetTransformFloat4x4(ED3DTRANSFORMSTATE eState) {
		return m_TransformMatrix[eState];
	}

	_float4x4 GetTransformMatrix(ED3DTRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_TransformMatrix[eState]);
	}

	_float4x4 GetTransformFloat4x4Inverse(ED3DTRANSFORMSTATE eState) {
		return m_TransformMatrix_Inverse[eState];
	}

	_float4x4 GetTransformMatrixInverse(ED3DTRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_TransformMatrix_Inverse[eState]);
	}

	_float4x4 GetPrevTransformMatrix(ED3DTRANSFORMSTATE eState) {
		return m_PrevTransformMatrix[eState];
	}

	_float4 GetCamPosition() {
		return m_vCamPosition;
	}

	_float3 GetCamLook() {
		return m_vCamLook;
	}

	_float GetFar() {
		return m_fFar;
	}

	_float4x4* GetShadowProjMat() 
	{
		return m_ShadowProjMatrix;
	}

	_float4x4* GetShadowViewMat()
	{
		return m_ShadowViewMatrix;
	}

	_float4x4* GetShadowWorldMat()
	{
		return m_ShadowReverseProjMat;
	}

public:
	void Tick();


private:
	/*Cascade Box 계산 함수*/
	void CascadeTransform();

private:
	_float4x4			m_TransformMatrix[D3DTS_END] = {};
	_float4x4			m_TransformMatrix_Inverse[D3DTS_END] = {};
	_float4x4			m_PrevTransformMatrix[D3DTS_END] = {};
	
	_float4				m_vCamPosition = {};
	_float3				m_vCamLook = {};

	_float				m_fFar = 0.f;
	_float				m_fNear = 0.f;
	_float				m_fFov = 0.f;

private:
	_float4x4			m_ShadowProjMatrix[3];
	_float4x4			m_ShadowViewMatrix[3];
	_float4x4			m_ShadowReverseProjMat[3];


public:

	static shared_ptr<CPipeLine> Create();


};

END