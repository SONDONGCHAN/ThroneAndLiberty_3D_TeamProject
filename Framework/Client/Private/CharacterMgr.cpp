#include "CharacterMgr.h"
#include "CameraSpherical.h"
#include "RigidBody.h"

IMPLEMENT_SINGLETON(CCharacterMgr)

CCharacterMgr::CCharacterMgr()
{
}

CCharacterMgr::~CCharacterMgr()
{
}

void CCharacterMgr::AddPolymorph(const string& _strPolymorphTag, shared_ptr<CPolymorph> _pPolymorph)
{
	m_Polymorphs.emplace(_strPolymorphTag, _pPolymorph);
}

void CCharacterMgr::SetCurrentPolymorph(const string& _strPolymorphTag)
{
	auto iter = m_Polymorphs.find(_strPolymorphTag);
	
	if (iter != m_Polymorphs.end())
	{
		m_CurrentPolymorph = iter->second;
	}
}

void CCharacterMgr::SetCamera(shared_ptr<CCameraSpherical> _pCameraSpherical)
{
	m_pCamera = _pCameraSpherical;
}

HRESULT CCharacterMgr::ReadyLight()
{
	/*Point Light*/
	LIGHT_DESC		LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4();
	LightDesc.fRange = 4.f;

	LightDesc.vDiffuse = _float4(0.85f, 0.85f, 0.85f, 1.f);
	LightDesc.vAmbient = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.bUsingNoise = false;
	LightDesc.fCullDistance = 50.f;

	GAMEINSTANCE->AddLight(LightDesc, &m_iLightIdxFront);


	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4();
	LightDesc.fRange = 4.f;

	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.bUsingNoise = false;
	LightDesc.fCullDistance = 50.f;

	GAMEINSTANCE->AddLight(LightDesc, &m_iLightIdxBack);

	for (auto& iter : m_Polymorphs) {

		iter.second->SetLightIdx(m_iLightIdxFront, m_iLightIdxBack);
		iter.second->SetIsInDungeon(true);
	}

	return S_OK;
}

void CCharacterMgr::SetInDungeon(_bool _bInDungeon)
{
	for (auto& iter : m_Polymorphs) {

		iter.second->SetIsInDungeon(_bInDungeon);
	}

}

void CCharacterMgr::ChangePolymorph(const string& _strPolymorphTag)
{
	if (nullptr == m_CurrentPolymorph.lock())
		return;

	_float4x4 OldWorldMat = m_CurrentPolymorph.lock()->GetTransform()->GetWorldMatrix();
	m_CurrentPolymorph.lock()->SetEnable(false);
	m_CurrentPolymorph.lock()->GetRigidBody()->SimulationOff();

	
	auto iter = m_Polymorphs.find(_strPolymorphTag);

	if (iter != m_Polymorphs.end()) 
	{
		m_CurrentPolymorph = iter->second;
		m_CurrentPolymorph.lock()->SetEnable(true);
		m_CurrentPolymorph.lock()->GetRigidBody()->SimulationOn();
		m_CurrentPolymorph.lock()->GetTransform()->SetWorldMatrix(OldWorldMat);
		m_CurrentPolymorph.lock()->GetTransform()->GoStraight(0.f);
		m_pCamera.lock()->SetOwner(dynamic_pointer_cast<CGameObject>(m_CurrentPolymorph.lock()));
		m_CurrentPolymorph.lock()->SetChanged();
		GAMEINSTANCE->PlaySoundW("Player_Poly_Base", 0.3f);
	}
}