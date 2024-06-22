#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVITerrain;
END

BEGIN(Tool_Map)
class CTerrain : public CGameObject
{

public:
	enum BLENDTYPE { BLEND_RED, BLEND_GREEN, BLEND_BLUE, BLEND_ALPHA, BLEND_END };

	struct WEIGHT_DESC
	{
		ComPtr<ID3D11ShaderResourceView> pWeightSRV{};
		array<string, BLEND_END> arrBlendDiffusesName{};
	};

private:
	CTerrain();
	DESTRUCTOR(CTerrain)

public:
	shared_ptr<CVITerrain> GetVIBuffer() { return m_pVIBufferCom; }
	void SetVIBuffer(shared_ptr<CVITerrain> _pVITerrain) { m_pVIBufferCom = _pVITerrain; }

	void SetBrushRange(_float _fBrushRadius) { m_fBrushRadius = _fBrushRadius; }
	void SetBrushPos(_float3 _vBrushPos) { m_vBrushPos = _vBrushPos; }
	
	string GetBaseDiffuseTextureName() { return m_strBaseDiffuseTextureName; }
	void SetBaseDiffuseTextureName(string& _strBaseDiffuseTextureName) { m_strBaseDiffuseTextureName = _strBaseDiffuseTextureName; }

	vector<WEIGHT_DESC>& GetWeightDescs() { return m_vecWeightDescs; }

	void ChangeViewMode();

public:
	virtual HRESULT Initialize(ifstream& _InFileStream);
	virtual HRESULT Initialize(_float4x4 _matWorld, shared_ptr<CVITerrain> _pVITerrain);

	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderPixelPicking();

private:
	_float m_fBrushRadius{}; // ¸ÊÅø
	_float3 m_vBrushPos{}; // ¸ÊÅø

	string m_strBaseDiffuseTextureName{}; // Å¬¶ó + ¸ÊÅø
	string m_strBrushTextureName{}; // ¸ÊÅø

	vector<WEIGHT_DESC> m_vecWeightDescs{}; // Å¬¶ó + ¸ÊÅø

	shared_ptr<CVITerrain> m_pVIBufferCom{}; // Å¬¶ó + ¸ÊÅø


public:
	static shared_ptr<CTerrain> Create(ifstream& _InFileStream);
	static shared_ptr<CTerrain> Create(_float4x4 _matWorld, shared_ptr<CVITerrain> _pVITerrain);
};
END