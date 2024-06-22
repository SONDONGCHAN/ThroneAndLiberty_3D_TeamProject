#include "GroupObject.h"
#include "EnvironmentObject.h"

CGroupObject::CGroupObject()
{
}

#ifdef CHECK_REFERENCE_COUNT
CGroupObject::~CGroupObject()
{
}
#endif // CHECK_REFERENCE_COUNT

HRESULT CGroupObject::Initialize(string _strGroupObjectName, vector<pair<string, _float4x4>> _vecMemberInfo)
{
    __super::Initialize(nullptr);

    m_pTransformCom->SetWorldMatrix(_float4x4::Identity);
    
    m_strGroupObjectName = _strGroupObjectName;

    m_vecMemberInfo = _vecMemberInfo;

    for (auto MemverInfo : _vecMemberInfo)
    {
        shared_ptr<CEnvironmentObject> pMemverObject = CEnvironmentObject::Create(MemverInfo.first, MemverInfo.second);
        //pMemverObject->SetOnerTransform(m_pTransformCom);
        pMemverObject->SetOnerGroupObject(static_pointer_cast<CGroupObject>(shared_from_this()));
        pMemverObject->SetIsGroup(true);
        m_vecMemberObjects.push_back(pMemverObject);
    }

	return S_OK;
}

HRESULT CGroupObject::Initialize(string _strGroupObjectName, _float4x4 _matGroupObjectWorld, vector<MEMBER_DESC> _vecMemberObjectDesc)
{
    __super::Initialize(nullptr);

    m_pTransformCom->SetWorldMatrix(_matGroupObjectWorld);

    m_strGroupObjectName = _strGroupObjectName;

    //m_vecMemberInfo = _vecMemberInfo;

    for (auto& MemberObjectDesc : _vecMemberObjectDesc)
    {
        shared_ptr<CEnvironmentObject> pMemverObject = CEnvironmentObject::Create(MemberObjectDesc.strModelKey, MemberObjectDesc.matWorld);
        pMemverObject->SetCenterPoint(MemberObjectDesc.vCenterPoint);
        pMemverObject->SetRadius(MemberObjectDesc.fRadius);
        pMemverObject->SetCollider();
        pMemverObject->SetOnerGroupObject(static_pointer_cast<CGroupObject>(shared_from_this()));
        pMemverObject->SetIsGroup(true);
        m_vecMemberObjects.push_back(pMemverObject);
        //pMemverObject->SetOnerTransform(m_pTransformCom);
        m_vecMemberInfo.push_back(make_pair(MemberObjectDesc.strModelKey, MemberObjectDesc.matWorld));

        //auto myPair = std::make_pair("Hello", 42);
        
    }

    return S_OK;
}

void CGroupObject::PriorityTick(_float _fTimeDelta)
{
    for (auto pMemberObjects : m_vecMemberObjects)
        pMemberObjects->PriorityTick(_fTimeDelta);
}

void CGroupObject::Tick(_float _fTimeDelta)
{
    for (auto pMemberObjects : m_vecMemberObjects)
        pMemberObjects->Tick(_fTimeDelta);
}

void CGroupObject::LateTick(_float _fTimeDelta)
{
    for (auto pMemberObjects : m_vecMemberObjects)
        pMemberObjects->LateTick(_fTimeDelta);
}

HRESULT CGroupObject::Render()
{
	return S_OK;
}

shared_ptr<CGroupObject> CGroupObject::Clone()
{
    shared_ptr<CGroupObject> pGroupObject = CGroupObject::Create(m_strGroupObjectName, m_vecMemberInfo);
    if (nullptr == pGroupObject)
        return nullptr;
   
    pGroupObject->GetTransform()->SetWorldMatrix(m_pTransformCom->GetWorldMatrix());
   
    return pGroupObject;
}

shared_ptr<CGroupObject> CGroupObject::Create(string _strGroupObjectName, vector<pair<string, _float4x4>> _vecMemberInfo)
{
    shared_ptr<CGroupObject> pInstance = WRAPPING(CGroupObject)();

    if (FAILED(pInstance->Initialize(_strGroupObjectName, _vecMemberInfo)))
    {
        MSG_BOX("Failed to Created : CGroupObject");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CGroupObject> CGroupObject::Create(string _strGroupObjectName, _float4x4 _matGroupObjectWorld, vector<MEMBER_DESC> _vecMemberObjectDesc)
{
    shared_ptr<CGroupObject> pInstance = WRAPPING(CGroupObject)();

    if (FAILED(pInstance->Initialize(_strGroupObjectName, _matGroupObjectWorld, _vecMemberObjectDesc)))
    {
        MSG_BOX("Failed to Created : CGroupObject");
        pInstance.reset();
    }

    return pInstance;
}