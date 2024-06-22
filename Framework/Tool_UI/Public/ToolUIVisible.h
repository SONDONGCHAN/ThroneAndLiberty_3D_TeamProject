#pragma once

#include "ToolUIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Tool_UI)

class CToolUIVisible : public CToolUIBase
{
	using Super = CToolUIBase;

public:
	CToolUIVisible();
	virtual ~CToolUIVisible();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	_float GetVisibleTopY() { return m_fVisibleTopY; }
	_float GetVisibleBotY() { return m_fVisibleBotY; }
	_float GetVisibleYSize() { return m_fVisibleYSize; }

	void SetLoadInitData(_float _fSizeY);

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CToolUIVisible> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);

private:
	_float2 m_vOriginPos{};
	_float m_fDiffDistFromOrigin{ 0.f };

private:
	_float m_fVisibleTopY{ 0.f };
	_float m_fVisibleBotY{ 0.f };
	_float m_fVisibleYSize{ 0.f };

};

END
