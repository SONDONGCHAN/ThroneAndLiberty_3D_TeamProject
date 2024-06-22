#include "Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

HRESULT CLayer::Initialize()
{
    return S_OK;
}

void CLayer::PriorityTick(_float _fTimeDelta)
{
    for (auto pGameObject = m_GameObjects.begin(); pGameObject != m_GameObjects.end(); ) {

        //Dead Check
        if (!((*pGameObject)->IsActive())) {
            pGameObject->reset();
            pGameObject = m_GameObjects.erase(pGameObject);
            continue;
        }

        if ((*pGameObject)->IsEnabled()) {
            (*pGameObject)->PriorityTick(_fTimeDelta);
        }
        pGameObject++;

    }

}

void CLayer::Tick(_float _fTimeDelta)
{

    for (auto& pGameObject : m_GameObjects) {

        if (pGameObject && pGameObject->IsEnabled())
            pGameObject->Tick(_fTimeDelta);
        
    }

}

void CLayer::LateTick(_float _fTimeDelta)
{
    for (auto pGameObject = m_GameObjects.begin(); pGameObject != m_GameObjects.end(); ) {

        if ((*pGameObject)->IsEnabled()) {
            (*pGameObject)->LateTick(_fTimeDelta);
        }
        pGameObject++;

    }

}

HRESULT CLayer::AddGameObject(shared_ptr<class CGameObject> _pGameObject)
{
    if (!_pGameObject)
        return E_FAIL;

    m_GameObjects.push_back(_pGameObject);

    return S_OK;
}

void CLayer::ClearLayer()
{
    m_GameObjects.clear();
}

shared_ptr<CGameObject> CLayer::GetGameObject()
{
    auto	iter = m_GameObjects.begin();

    return (*iter);
}

shared_ptr<CLayer> CLayer::Create()
{
    shared_ptr<CLayer> pInstance = make_shared<CLayer>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLayer");
    
    return pInstance;
}
