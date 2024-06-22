#pragma once

#include "Tool_Defines.h"
#include "AnimModel.h"

BEGIN(Engine)
class CModel;
class CAnimation;
END

BEGIN(Tool_Effect)

class CWindowModel
{
public:
	CWindowModel();
	~CWindowModel() = default;

public:
	HRESULT Initialize();
	void	Tick(_float _fTimeDelta);

private:
	// ·»´õ¿ë °´Ã¼
	shared_ptr<class CAnimModel>	m_pAnimModel = { nullptr };

	// ¸ðµ¨
	vector<string>			m_vecAnimModelName;
	_uint					m_iCurrentModelIndex = { 0 };
	shared_ptr<CModel>		m_pCurrentAnimModel = { nullptr };
	_bool					m_bObjectCreated = { false };

	// ¾Ö´Ï¸ÞÀÌ¼Ç
	vector<shared_ptr<class CAnimation>>	m_CurrentModelAnimations;
	_int									m_iCurrentAnimationIndex = { -1 };
	vector<const _char*>					m_vecAnimationNames;
	_bool									m_isAnimationPlay = { true };

	// º»
	vector<shared_ptr<class CBone>>			m_CurrentModelBones;
	vector<const _char*>					m_vecModelBoneNames;
	const _char*							m_CurrentBoneName;

	_bool									m_bShowEditWindow = true;

public:
	shared_ptr<class CAnimModel>	GetCurModelObj() { return m_pAnimModel; }
	const _char*					GetCurModelBone() { return m_CurrentBoneName; }

	vector<string>	GetAnimModelName() { return m_vecAnimModelName; }

	shared_ptr<CAnimModel>	ChangeModelAndAnim(string _strModelKey, string _strAnimKey);

private:
	void			KeyInput();
	HRESULT			LoadAllAnimModelName(const wstring& _strModelPath);

private:
	ComPtr<ID3D11Device> m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
};

END