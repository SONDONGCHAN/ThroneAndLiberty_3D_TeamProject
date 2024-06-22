#include "ObjectMgr.h"
#include "Layer.h"
#include "PhysXMgr.h"

#include "GameInstance.h"

CObjectMgr::CObjectMgr()
{
}

CObjectMgr::~CObjectMgr()
{
    Free();
}

HRESULT CObjectMgr::Initialize(_uint _iNumLevels)
{
    m_pLayers = new LAYERS[_iNumLevels];
    m_iNumLevels = _iNumLevels;

    return S_OK;
}

void CObjectMgr::PriorityTick(_float fTimeDelta)
{
    for (UINT i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pLayers[i])
            Pair.second->PriorityTick(fTimeDelta);
    }

}

void CObjectMgr::Tick(_float fTimeDelta)
{

    for (UINT i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pLayers[i])
            Pair.second->Tick(fTimeDelta);
    }

}

void CObjectMgr::LateTick(_float fTimeDelta)
{
    //레벨전환 
    if (m_IsChangeSignalOn) {

        m_IsChangeSignalOn = false;
        GAMEINSTANCE->SetSimultation(false);
        GAMEINSTANCE->OpenLevelForClient(m_iChangeLevel);
    }


    for (UINT i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pLayers[i])
            Pair.second->LateTick(fTimeDelta);
    }


}

HRESULT CObjectMgr::AddObject(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<CGameObject> _pGameObject)
{
    if (!_pGameObject)
        return E_FAIL;


    shared_ptr<CLayer> pLayer = FindLayer(iLevelIndex, strLayerTag);

    if (!pLayer) {

        pLayer = CLayer::Create();

        if (!pLayer)
            return E_FAIL;

        pLayer->AddGameObject(_pGameObject);
        m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
    }
    else {
        pLayer->AddGameObject(_pGameObject);
    }

    return S_OK;
}

shared_ptr<class CGameObject> CObjectMgr::GetGameObject(_uint iLevelIndex, const wstring& strLayerTag)
{
    shared_ptr<CLayer> pLayer = FindLayer(iLevelIndex, strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->GetGameObject();
}

list<shared_ptr<class CGameObject>>* CObjectMgr::GetObjectList(_uint iLevelIndex, const wstring& strLayerTag)
{
    shared_ptr<CLayer> pLayer = FindLayer(iLevelIndex, strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->GetObjectList();
}

shared_ptr<class CLayer> CObjectMgr::FindLayer(_uint _iLevelIndex, const wstring& _strLayerTag)
{
    if (_iLevelIndex >= m_iNumLevels || m_pLayers[_iLevelIndex].empty())
        return nullptr;

    auto iter = m_pLayers[_iLevelIndex].find(_strLayerTag);

    if (m_pLayers[_iLevelIndex].end() == iter)
        return nullptr;

    return iter->second;
}

void CObjectMgr::ClearLayer(_uint _iLevelIndex, const wstring& _strLayerTag)
{
    shared_ptr<CLayer> pLayer = FindLayer(_iLevelIndex, _strLayerTag);

    if (pLayer)
        pLayer->ClearLayer();
}

shared_ptr<CObjectMgr> CObjectMgr::Create(_uint iNumLevels)
{
    shared_ptr<CObjectMgr> pInstance = make_shared<CObjectMgr>();

    if (pInstance->Initialize(iNumLevels))
        MSG_BOX("Failed to Create : CObjectMgr");

    return pInstance;
}

void CObjectMgr::Clear(_uint _iLevelNum)
{
    for (UINT i = 1; i < m_iNumLevels; i++)
    {
        m_pLayers[i].clear();
    }
}

void CObjectMgr::Clear()
{
    for (UINT i = 0; i < m_iNumLevels; i++)
    {
        m_pLayers[i].clear();
    }
}

void CObjectMgr::Free()
{
    for (UINT i = 0; i < m_iNumLevels; i++)
    {
        m_pLayers[i].clear();
    }

    delete[] m_pLayers;

}

