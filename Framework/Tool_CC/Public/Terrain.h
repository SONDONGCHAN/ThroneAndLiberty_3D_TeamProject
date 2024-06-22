#pragma once

#include "Tool_CC_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVITerrain;
END

class CTerrain : public CGameObject
{
public:
	enum BLENDTYPE { BLEND_RED, BLEND_GREEN, BLEND_BLUE, BLEND_ALPHA, BLEND_END };

	struct WEIGHT_DESC
	{
		ComPtr<ID3D11ShaderResourceView> pWeightSRV{};
		array<string, BLEND_END> arrBlendDiffusesName{};
	};
public:
	CTerrain();
	~CTerrain();

public:
	string GetBaseDiffuseTextureName() { return m_strBaseDiffuseTextureName; }

public:
	virtual HRESULT Initialize(ifstream& _InFileStream, _bool _IsField);

	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	string m_strBaseDiffuseTextureName{};
	vector<WEIGHT_DESC> m_vecWeightDescs{};

	shared_ptr<CVITerrain> m_pVIBufferCom{};

	shared_ptr<CRigidBody> m_pRigidBody{};

public:
	static shared_ptr<CTerrain> Create(ifstream& _InFileStream, _bool _IsField);
};