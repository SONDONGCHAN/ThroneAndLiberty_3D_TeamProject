#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVITerrain;
class CRigidBody;
END

BEGIN(Client)
class CTestTerrain : public CGameObject
{

public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };

public:
	CTestTerrain();
	virtual ~CTestTerrain();;

public:
	virtual HRESULT Initialize(string _strTexKey, string _strFilePath);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	string m_pTextures[TYPE_END]{};

	shared_ptr<CVITerrain> m_pVIBufferCom{};
	shared_ptr<CRigidBody> m_pRigidBody{};

public:
	static shared_ptr<CTestTerrain> Create(string _strTexKey, string _strFilePath);

};
END