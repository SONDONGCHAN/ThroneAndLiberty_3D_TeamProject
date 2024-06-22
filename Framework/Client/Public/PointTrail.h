#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIPointTrail;
END

BEGIN(Client)

class CPointTrail : public CGameObject
{
public:
	CPointTrail();
	CPointTrail(const CPointTrail& rhs);
	virtual ~CPointTrail() = default;

public:
	virtual HRESULT Initialize(const string& _strTextureKey);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderGlow() override;
	virtual HRESULT RenderDistortion() override;

public:
	void	SetColor(_float4 _vUpColor, _float4 _vDownColor);
	void	ChangeWeapon(shared_ptr<CGameObject> _pOwner, _float3 _vUpOffset, _float3 _vDownOffset);
	void	ClearTrail();

public:
	void	StartTrail(_float4 _vUpColor, _float4 _vDownColor, _bool _bGlow = true, _bool _bDistortion = true, _bool _bAlphaChange = false, _float _fChangeWeight = 0.f);
	void	EndTrail();

private:
	shared_ptr<CVIPointTrail>	m_pVIBufferCom = nullptr;
	string						m_strTextureKey = "";

	_float4x4*					m_pSocketMatrix = nullptr;
	shared_ptr<CGameObject>		m_pOwner;
	_float4x4					m_pOwnerMatrix;

	_float3						m_vUpOffset = { 0.f, 0.f, 0.f };
	_float3						m_vDownOffset = { 0.f, 0.f, 0.f };

	_float						m_fLifeTme = 0.0f;

	_float4						m_vUpColor = XMVectorSet(1.f, 0.28f, 0.3f, 1.f);
	_float4						m_vDownColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	_float						m_fAlpha = 1.f;
	_bool						m_bAlphaChange = false;
	_bool						m_bEnding = false;
	_bool						m_bStarting = false;
	_float						m_fChangeWeight = 0.f;

	_bool						m_bGlow = false;
	_bool						m_bDistortion = false;

	_float						m_fDistortionTime = 0.f;
	string						m_strDistortionKey = "Noise_6509";
	_float						m_fDistortionSpeed = 1.f;
	_float						m_fDistortionWeight = 1.f;

public:
	static shared_ptr<CPointTrail> Create(const string& _strTextureKey);
};

END