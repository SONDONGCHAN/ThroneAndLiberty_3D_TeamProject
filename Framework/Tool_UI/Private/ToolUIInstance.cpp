#include "pch.h"
#include "ToolUIInstance.h"
#include "ToolUIItemTab.h"

#include "GameInstance.h"
#include "VIInstanceUI.h"

CToolUIInstance::CToolUIInstance()
{
}

CToolUIInstance::~CToolUIInstance()
{
}

HRESULT CToolUIInstance::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIInstance::UIInstDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _uiPos, vector<_float2>& _uiSize)
{
    CTransform::TRANSFORM_DESC pGameObjectDesc;

    pGameObjectDesc.fSpeedPerSec = 10.f;
    pGameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(Super::Initialize(&pGameObjectDesc)))
        return E_FAIL;

    //m_InstanceDesc = _InstanceDesc;
    m_UIDesc = _UIDesc;
    m_uiPos = _uiPos;
    m_uiSize = _uiSize;
    
    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc, _uiPos, _uiSize)))
        return E_FAIL;

    m_eShaderType = ESHADER_TYPE::ST_UI;
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    return S_OK;
}

void CToolUIInstance::PriorityTick(_float _deltaTime)
{

}

void CToolUIInstance::Tick(_float _deltaTime)
{
    KeyInput();

    if (m_strTexKeys.size() > 1 && IsMouseOn())
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;

    /* 부모를 따라다니게끔 */
    if (!m_pUIParent.expired() && (m_pUIParent.lock()->GETPOS != m_vPrevParentPos) && (m_vPrevParentPos != _float3(FLT_MAX, FLT_MAX, FLT_MAX)))
    {
        _float3 moveDir = m_pUIParent.lock()->GETPOS - m_vPrevParentPos;
        moveDir.Normalize();
        _float moveDist = SimpleMath::Vector3::Distance(m_pUIParent.lock()->GETPOS, m_vPrevParentPos);

        _float3 newPos = _float3(m_fX, m_fY, 0.f) + (moveDir * moveDist);
        SetUIPos(newPos.x, newPos.y);

        m_BindWorldMat = m_pTransformCom->GetWorldMatrix();
    }
    else
        m_BindWorldMat = m_pTransformCom->GetWorldMatrix();

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.lock() != nullptr)
                childPair.lock()->Tick(_deltaTime);

            if (childPair.lock()->GetUIType() == TOOL_UI_ITEMTAB)
            {
                weak_ptr<CToolUIItemTab> childTab = dynamic_pointer_cast<CToolUIItemTab>(childPair.lock());
                if (!childTab.expired())
                {
                    if (childTab.lock()->IsTabActivated())
                        m_iTexIndex = 1;
                }
            }
        }
    }

    if (!m_pUIParent.expired())
        m_vPrevParentPos = m_pUIParent.lock()->GETPOS;
}

void CToolUIInstance::LateTick(_float _deltaTime)
{
    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
        return;

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.lock() != nullptr)
                childPair.lock()->LateTick(_deltaTime);
        }
    }
}

HRESULT CToolUIInstance::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    //if (FAILED(GAMEINSTANCE->BindRawValue("g_iHF", &m_isRight, sizeof(_uint))))
    //    return E_FAIL;

    if (!m_strTexKeys.empty())
    {
        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;
    }

    if (m_UIDesc.isMasked)
    {
        if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
            return E_FAIL;
    }

    if (m_UIDesc.isNoised)
    {
        if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
            return E_FAIL;
    }

    if (FAILED(GAMEINSTANCE->BeginShader()))
        return E_FAIL;

    if (FAILED(m_pInstanceBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pInstanceBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

void CToolUIInstance::SetUIPos(_float _fPosX, _float _fPosY)
{
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(_fPosX, _fPosY, 0.f));
    m_fX = _fPosX;
    m_fY = _fPosY;
}

void CToolUIInstance::SetBufferSize(_float _fSizeX, _float _fSizeY)
{
    m_pInstanceBuffer->SetInstanceBufferScale(_fSizeX, _fSizeY);
    m_fSizeX = _fSizeX;
    m_fSizeY = _fSizeY;
    m_InstanceDesc.vSize = _float2(m_fSizeX, _fSizeY);
}

void CToolUIInstance::SetColor(_float4 _vColor)
{
    m_pInstanceBuffer->SetInstanceColor(_vColor);
    m_InstanceDesc.vColor = _vColor;
}

_bool CToolUIInstance::IsFinished()
{
    if (m_pBuffer != nullptr)
        return m_pInstanceBuffer->IsFinished();

    return false;
}

_bool CToolUIInstance::IsMouseOn()
{
    POINT mousePt;
    ::GetCursorPos(&mousePt);
    ::ScreenToClient(g_hWnd, &mousePt);

    _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
    _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

    if ((mousePosX >= (m_fX - m_fSizeX * 0.5f)) && (mousePosX <= (m_fX + m_fSizeX * 0.5f)) && (mousePosY >= (m_fY - m_fSizeY * 0.5f) &&
        mousePosY <= m_fY + m_fSizeY * 0.5f))
        return true;

    return false;
}

shared_ptr<CGameObject> CToolUIInstance::GetUIOwner()
{
    if (!m_pOwner.expired())
        return m_pOwner.lock();

    return nullptr;
}

_bool CToolUIInstance::IsChild()
{
    if (!m_pUIParent.expired())
        return true;

    return false;
}

shared_ptr<CToolUIInstance> CToolUIInstance::GetUIParent()
{
    if (m_pUIParent.expired())
        return nullptr;

    return m_pUIParent.lock();
}

void CToolUIInstance::SetShaderPassIndex(_uint _passIndex)
{
    m_UIDesc.iShaderPassIdx = _passIndex;
    m_iShaderPass = _passIndex;
}

void CToolUIInstance::AddUIChild(shared_ptr<CToolUIInstance> _pUIChild)
{
    shared_ptr<CToolUIInstance> uiChild = _pUIChild;
    auto it = find_if(m_pUIChildren.begin(), m_pUIChildren.end(), [&uiChild](weak_ptr<CToolUIInstance> _pInstanceUI) {
        if (uiChild->GetUITag() == _pInstanceUI.lock()->GetUITag())
            return true;

        return false;
        });

    if (it != m_pUIChildren.end())
        return;

    m_pUIChildren.emplace_back(_pUIChild);
}

void CToolUIInstance::AddUIParent(shared_ptr<CToolUIInstance> _uiParent)
{
    m_pUIParent = _uiParent;

    m_vPrevParentPos = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
}

void CToolUIInstance::RemoveUIChild(const string& _strUIChildTag)
{
    string childUITag = _strUIChildTag;
    auto it = find_if(m_pUIChildren.begin(), m_pUIChildren.end(), [&childUITag](weak_ptr<CToolUIInstance> _pInstanceUI) {
        if (childUITag == _pInstanceUI.lock()->GetUITag())
            return true;

        return false;
        });

    if (it == m_pUIChildren.end())
        return;

    m_pUIChildren.erase(it);
}

void CToolUIInstance::RemoveAllUIChild()
{
    for (auto& uiChild : m_pUIChildren)
    {
        if (!uiChild.expired())
            uiChild.reset();
    }

    m_pUIChildren.clear();
}

void CToolUIInstance::RemoveUIParent()
{
    m_pUIParent.reset();
}

void CToolUIInstance::RotateUI(_float _xValue, _float _yValue, _float _zValue)
{
    m_pTransformCom->RotationAll(XMConvertToRadians(_xValue), XMConvertToRadians(_yValue), XMConvertToRadians(_zValue));
}

void CToolUIInstance::ActivateUI()
{
    //ResetInstanceData();
    m_isActivated = true;
    m_fSelectedZOrder = 0.f;
}

void CToolUIInstance::DeactivateUI()
{
    m_isActivated = false;
    m_isSelected = false;
}

void CToolUIInstance::ResetInstanceData()
{

}

void CToolUIInstance::ConvertMousePosToUV(_float _mouseX, _float _mouseY)
{
    _float mousePosX = _mouseX - g_iWinSizeX * 0.5f;
    _float mousePosY = -_mouseY + g_iWinSizeY * 0.5f;

    if ((mousePosX >= (m_fX - m_fSizeX * 0.5f)) && (mousePosX <= (m_fX + m_fSizeX * 0.5f)) && (mousePosY >= (m_fY - m_fSizeY * 0.5f) &&
        mousePosY <= m_fY + m_fSizeY * 0.5f))
    {
        float newMinX = (m_fX - (m_fSizeX * 0.5f));
        float newMaxX = (m_fX + (m_fSizeX * 0.5f));
        float newMinY = (m_fY - (m_fSizeY * 0.5f));
        float newMaxY = (m_fY + (m_fSizeY * 0.5f));

        m_uvMouse.x = (mousePosX - newMinX) / (fabs(newMinX) + fabs(newMaxX));
        m_uvMouse.y = (-mousePosY + newMaxY) / (fabs(newMinY) + fabs(newMaxY));
    }
}

void CToolUIInstance::PrepareUVs(_float _fX, _float _fY)
{
    for (_uint i = 0; i < static_cast<_uint>(_fY); ++i)
    {
        for (_uint j = 0; j < static_cast<_uint>(_fX); ++j)
        {
            m_UVs.emplace_back(_float4(static_cast<_float>(j) / _fX, static_cast<_float>(i) / _fY, static_cast<_float>(j + 1) / _fX, static_cast<_float>(i + 1) / _fY));
        }
    }
}

void CToolUIInstance::KeyInput()
{
    if (!m_pUIParent.expired())
    {
        if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT) && GAMEINSTANCE->MouseDown(DIM_LB) && IsMouseOn())
        {
            POINT mousePt;
            ::GetCursorPos(&mousePt);
            ::ScreenToClient(g_hWnd, &mousePt);

            _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
            _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

            m_vDir = _float3(mousePosX, mousePosY, 0.f) - _float3(m_BindWorldMat._31, m_BindWorldMat._32, 0.f);
            m_fDist = m_vDir.Length();
            m_vDir.Normalize();
        }

        if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT) && GAMEINSTANCE->MousePressing(DIM_LB))
        {
            POINT mousePt;
            ::GetCursorPos(&mousePt);
            ::ScreenToClient(g_hWnd, &mousePt);

            _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
            _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

            _float3 dir = _float3(mousePosX, mousePosY, 0.f) - _float3(m_BindWorldMat._31, m_BindWorldMat._32, 0.f);
            _float dist = dir.Length();
            dir.Normalize();

            if (dist != m_fDist)
            {
                _float3 moveDir = m_vDir * m_fDist;
                _float3 newPos = _float3(mousePosX, mousePosY, 0.f) - moveDir;
                newPos = newPos.Transform(newPos, m_pUIParent.lock()->GetTransform()->GetWorldMatrixInverse());

                //m_vLocalPos = newPos;
            }
        }
    }

    else
    {
        /*if (GAMEINSTANCE->MouseDown(DIM_LB))
        {
            POINT mousePt;
            ::GetCursorPos(&mousePt);
            ::ScreenToClient(g_hWnd, &mousePt);

            _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
            _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

            m_vDir = _float3(mousePosX, mousePosY, 0.f) - m_pTransformCom->GetState(CTransform::STATE_POSITION);
            m_fDist = m_vDir.Length();
            m_vDir.Normalize();

            return;
        }

        if (GAMEINSTANCE->MousePressing(DIM_LB) && m_isSelected)
        {
            POINT mousePt;
            ::GetCursorPos(&mousePt);
            ::ScreenToClient(g_hWnd, &mousePt);

            _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
            _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

            _float3 dir = _float3(mousePosX, mousePosY, 0.f) - m_pTransformCom->GetState(CTransform::STATE_POSITION);
            _float dist = dir.Length();
            dir.Normalize();

            if (dist != m_fDist)
            {
                _float3 moveDir = m_vDir * m_fDist;

                _float3 newPos = _float3(mousePosX, mousePosY, 0.f) - moveDir;

                SetUIPos(newPos.x, newPos.y);
            }
        }*/
    }
}

HRESULT CToolUIInstance::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _uiPos, vector<_float2>& _uiSize)
{
    m_pInstanceBuffer = CVIInstanceUI::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), _InstanceDesc, _uiPos);
    if (m_pInstanceBuffer == nullptr)
        return E_FAIL;

    m_strTexKeys = _strTextureTags;

    if (m_UIDesc.isMasked)
        m_strMaskKey = _strMaskTextureTag;

    if (m_UIDesc.isNoised)
        m_strNoiseKey = _strNoiseTextureTag;

    return S_OK;
}

shared_ptr<CToolUIInstance> CToolUIInstance::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIInstance::UIInstDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _uiPos, vector<_float2>& _uiSize)
{
    shared_ptr<CToolUIInstance> pInstance = make_shared<CToolUIInstance>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _uiPos, _uiSize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CToolUIInstance::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
