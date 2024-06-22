#pragma once

#include "Component.h"

BEGIN(Engine)

class CCamera;
class CCameraProduction;

class ENGINE_DLL CFilm final : public CComponent
{
public:
	CFilm(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CFilm() = default;

public:
	static shared_ptr<CFilm> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const wstring& _wstrFilePath);

public:
	HRESULT Initialize(const wstring& _wstrFilePath);

public:
	bool	isFinished() { return m_isFinished; }
	HRESULT PrepareFilm();
	HRESULT PrepareFilm(const _float3& _vLook, const _float3& _vPosition);
	HRESULT PlayProduction(float _fTimeDelta, shared_ptr<CCamera> _pCamera, bool _isPlayOriginal = true);
	HRESULT EndProduction();
	void	GetEndEyeAt(_float3* _vEye, _float3* _vAt, bool _isOriginal);

private:
	void CatmullRomFunc(shared_ptr<CCamera> _pCamera, int _i0, int _i1, int _i2, int _i3, float _fRatio, bool _isOriginal = true);
	void LinearFunc(shared_ptr<CCamera> _pCamera, float _fRatio, bool _isOriginal = true);

private:
	bool m_isFinished{ true };
	int m_iCurrentProduction{ 0 };
	float m_fTrackPosition{ 0.0f };
	bool m_isDoneFade{ false };
	bool m_isDoneShake{ false };
	vector<shared_ptr<CCameraProduction>> m_OriginalCameraProductions;
	vector<shared_ptr<CCameraProduction>> m_ConvertCameraProductions;
};

END