#include "pch.h"
#include "ToolUIScroll.h"
#include "ToolUISlot.h"
#include "ToolUIBG.h"

#include "GameInstance.h"
#include "Texture.h"
#include "VIInstancePoint.h"

CToolUIScroll::CToolUIScroll()
{

}

CToolUIScroll::~CToolUIScroll()
{

}

HRESULT CToolUIScroll::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    CTransform::TRANSFORM_DESC pGameObjectDesc;

    pGameObjectDesc.fSpeedPerSec = 10.f;
    pGameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(Super::Initialize(&pGameObjectDesc)))
        return E_FAIL;

    m_ToolUIDesc = _ToolUIDesc;
    m_InstanceDesc = _InstanceDesc;

    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc)))
        return E_FAIL;

    if (_initPos == _float2(0.f, 0.f))
    {
        m_fX = _InstanceDesc.vCenter.x;
        m_fY = _InstanceDesc.vCenter.y;
    }

    else
    {
        m_fX = _initPos.x;
        m_fY = _initPos.y;
    }

    m_fSizeX = _InstanceDesc.vSize.x;
    m_fSizeY = _InstanceDesc.fSizeY;
    m_vOriginPos = _float2(m_fX, m_fY);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UI;
    m_iShaderPass = 0;
    m_ToolUIDesc.iShaderPassIdx = 0;

    /* Initial Data Setting */
    /* Inventory BackGround의 정보를 가져와 세팅할 필요가 있다 */
    m_fVisibleYSize = (m_fSizeY * 2.f) / 3.f;
    m_vVisibleCenter = _float2(m_fX, (m_fY + (m_fSizeY / 2.f)) - (m_fVisibleYSize / 2.f));

    //m_fVisibleUVPosY = (m_fVisibleYSize / 2.f) / m_fSizeY;
    m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - (m_vVisibleCenter.y)) / m_fSizeY;
    m_fVisibleUVSizeY = m_fVisibleYSize / m_fSizeY;

    return S_OK;
}

void CToolUIScroll::PriorityTick(_float _fDeltaTime)
{

}

void CToolUIScroll::Tick(_float _fDeltaTime)
{
    KeyInput();

    ScrollKeyInput();

    //UpdateVisibleRange();

    if (!m_pUIParent.expired() && (m_pUIParent.lock()->GETPOS != m_vPrevParentPos) && (m_vPrevParentPos != _float3(FLT_MAX, FLT_MAX, FLT_MAX)))
    {
        if (m_isScrolling == false)
        {
            _float3 moveDir = m_pUIParent.lock()->GETPOS - m_vPrevParentPos;
            moveDir.Normalize();
            _float moveDist = SimpleMath::Vector3::Distance(m_pUIParent.lock()->GETPOS, m_vPrevParentPos);

            _float3 newPos = _float3(m_fX, m_fY, 0.f) + (moveDir * moveDist);
            SetUIPos(newPos.x, newPos.y);
        }

        m_BindWorldMat = m_pTransformCom->GetWorldMatrix();
    }
    else
        m_BindWorldMat = m_pTransformCom->GetWorldMatrix();

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
                childPair.second.lock()->Tick(_fDeltaTime);
        }
    }

    if (!m_pUIParent.expired())
        m_vPrevParentPos = m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
}

void CToolUIScroll::LateTick(_float _fDeltaTime)
{
    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
        return;

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
                childPair.second.lock()->LateTick(_fDeltaTime);
        }
    }
}

HRESULT CToolUIScroll::Render()
{
    //if (!m_pUIParent.expired())
    //{
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fVisiblePosY", &m_fVisibleUVPosY, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fVisibleSizeY", &m_fVisibleUVSizeY, sizeof(_float))))
            return E_FAIL;

        switch (m_ToolUIDesc.m_eUITexType)
        {
        case TOOL_UI_TEX_D:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
                return E_FAIL;
        }
        break;
        case TOOL_UI_TEX_DM:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
                return E_FAIL;

            if (m_ToolUIDesc.m_isMasked)
            {
                if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                    return E_FAIL;
            }

        }
        break;
        case TOOL_UI_TEX_M:
        {
            if (m_ToolUIDesc.m_isMasked)
            {
                if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                    return E_FAIL;

                if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                    return E_FAIL;
            }
        }
        break;
        case TOOL_UI_TEX_NONE:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;
        }
        break;
        case TOOL_UI_TEX_DMN:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
                return E_FAIL;

            if (m_ToolUIDesc.m_isMasked)
            {
                if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                    return E_FAIL;
            }

            if (m_ToolUIDesc.isNoised)
            {
                if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                    return E_FAIL;
            }
        }
        break;
        case TOOL_UI_TEX_DN:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
                return E_FAIL;

            if (m_ToolUIDesc.isNoised)
            {
                if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                    return E_FAIL;
            }
        }
        break;
        case TOOL_UI_TEX_N:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (m_ToolUIDesc.isNoised)
            {
                if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                    return E_FAIL;
            }
        }
        break;
        case TOOL_UI_TEX_MN:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (m_ToolUIDesc.m_isMasked)
            {
                if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                    return E_FAIL;
            }

            if (m_ToolUIDesc.isNoised)
            {
                if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                    return E_FAIL;
            }
        }
        break;
        }

        if (FAILED(GAMEINSTANCE->BeginShader()))
            return E_FAIL;

        if (FAILED(m_pBuffer->BindBuffers()))
            return E_FAIL;

        if (FAILED(m_pBuffer->Render()))
            return E_FAIL;

        return S_OK;
    //}

    /*else
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, 0)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        switch (m_ToolUIDesc.m_eUITexType)
        {
        case TOOL_UI_TEX_D:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (FAILED(m_pTextures[m_iTexIndex]->BindShaderResource("g_DiffuseTexture", 0)))
                return E_FAIL;
        }
        break;
        case TOOL_UI_TEX_DM:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (FAILED(m_pTextures[m_iTexIndex]->BindShaderResource("g_DiffuseTexture", 0)))
                return E_FAIL;

            if (m_pMaskTexture != nullptr)
            {
                if (FAILED(m_pMaskTexture->BindShaderResource("g_MaskTexture", 0)))
                    return E_FAIL;
            }

        }
        break;
        case TOOL_UI_TEX_M:
        {
            if (m_pMaskTexture != nullptr)
            {
                if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                    return E_FAIL;

                if (FAILED(m_pMaskTexture->BindShaderResource("g_MaskTexture", 0)))
                    return E_FAIL;
            }
        }
        break;
        case TOOL_UI_TEX_NONE:
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;
        }
        break;
        }

        if (FAILED(GAMEINSTANCE->BeginShader()))
            return E_FAIL;

        if (FAILED(m_pBuffer->BindBuffers()))
            return E_FAIL;

        if (FAILED(m_pBuffer->Render()))
            return E_FAIL;

        return S_OK;
    }*/
}

void CToolUIScroll::SetVisibleCenterAndSizeY(_float2 _vVisibleCenter, _float _fVisibleYSize)
{
    m_vParentCenter = _vVisibleCenter;
    m_fParentYSize = _fVisibleYSize;

    m_fVisibleYSize = (_fVisibleYSize * 2.f) / 3.f;
    m_vVisibleCenter = _float2(_vVisibleCenter.x, (_vVisibleCenter.y + (_fVisibleYSize / 2.f)) - (m_fVisibleYSize / 2.f));
    
    m_fVisibleUVPosY = ((_vVisibleCenter.y + (_fVisibleYSize / 2.f)) - (m_vVisibleCenter.y)) / _fVisibleYSize;
    m_fVisibleUVSizeY = m_fVisibleYSize / _fVisibleYSize;
}

void CToolUIScroll::UpdateVisibleRange()
{
    if (!m_pUIParent.expired())
    {
        if (m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) != m_vPrevParentPos && m_vPrevParentPos != _float3(FLT_MAX, FLT_MAX, FLT_MAX))
        {
            _float2 movedDir = _float2(m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) - _float2(m_vPrevParentPos.x, m_vPrevParentPos.y));
            movedDir.Normalize();
            m_fDifDistFromOrigin = SimpleMath::Vector2::Distance(_float2(m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION)), _float2(m_vPrevParentPos.x, m_vPrevParentPos.y));

            m_vVisibleCenter += movedDir * m_fDifDistFromOrigin;
            _float2 newPos = _float2(m_fX, m_fY) + (movedDir * m_fDifDistFromOrigin);
            SetUIPos(newPos.x, newPos.y);
        }

        m_fVisibleUVPosY = ((m_vParentCenter.y + (m_fParentYSize / 2.f)) - (m_vVisibleCenter.y)) / m_fParentYSize;
    }

    /* Origin */
    //m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - (m_vVisibleCenter.y)) / m_fSizeY;
}

void CToolUIScroll::ScrollKeyInput()
{
    if (!m_pUIParent.expired())
    {
        /*if (GAMEINSTANCE->MouseDown(DIM_RB) && IsScrollMouseOn())
        {
            POINT mousePt;
            ::GetCursorPos(&mousePt);
            ::ScreenToClient(g_hWnd, &mousePt);

            m_isSelected = true;

            _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
            _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

            m_vPickedPos = _float2(mousePosX, mousePosY);
        }

        if (GAMEINSTANCE->MousePressing(DIM_RB) && m_isSelected)
        {
            POINT mousePt;
            ::GetCursorPos(&mousePt);
            ::ScreenToClient(g_hWnd, &mousePt);

            _float mousePosX = mousePt.x - g_iWinSizeX / 2.f;
            _float mousePosY = -mousePt.y + g_iWinSizeY / 2.f;

            _float2 newMousePos = _float2(mousePosX, mousePosY);
            _float dist = SimpleMath::Vector2::Distance(newMousePos, m_vPickedPos);

            if ((newMousePos.y - m_vPickedPos.y) <= 0.f) // Scroll의 Top이 Visible 영역의 Top의 범위를 넘어가려 할 때
            {
                m_vLocalPos = _float3(m_vLocalPos.x, m_vLocalPos.y - (dist * 0.01f), 0.f);
                if ((m_vLocalPos.y + (m_fSizeY / 2.f)) <= (m_vVisibleCenter.y + (m_fVisibleYSize / 2.f)))
                {
                    m_vLocalPos = _float3(m_vLocalPos.x, (m_vVisibleCenter.y + (m_fVisibleYSize / 2.f)) - (m_fSizeY / 2.f), 0.f);
                }
            }

            else // Scroll의 Bottom이 Visible 영역의 Bottom의 범위를 넘어가려 할 때
            {
                m_vLocalPos = _float3(m_vLocalPos.x, m_vLocalPos.y + (dist * 0.01f), 0.f);
                if ((m_vLocalPos.y - (m_fSizeY / 2.f)) >= (m_vVisibleCenter.y - (m_fVisibleYSize / 2.f)))
                {
                    m_vLocalPos = _float3(m_vLocalPos.x, (m_vVisibleCenter.y - (m_fVisibleYSize / 2.f)) + (m_fSizeY / 2.f), 0.f);
                }
            }
        }

        if (GAMEINSTANCE->MouseUp(DIM_RB))
        {
            m_isSelected = false;
        }*/

        if (GAMEINSTANCE->MouseDown(DIM_RB) && IsScrollMouseOn())
        {
            POINT mousePt;
            ::GetCursorPos(&mousePt);
            ::ScreenToClient(g_hWnd, &mousePt);

            m_isSelected = true;

            _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
            _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

            m_vPickedPos = _float2(mousePosX, mousePosY);
        }

        if (GAMEINSTANCE->MousePressing(DIM_RB) && m_isSelected)
        {
            m_isScrolling = true;

            POINT mousePt;
            ::GetCursorPos(&mousePt);
            ::ScreenToClient(g_hWnd, &mousePt);

            _float mousePosX = mousePt.x - g_iWinSizeX / 2.f;
            _float mousePosY = -mousePt.y + g_iWinSizeY / 2.f;

            _float2 newMousePos = _float2(mousePosX, mousePosY);
            _float dist = SimpleMath::Vector2::Distance(newMousePos, m_vPickedPos);

            if ((newMousePos.y < m_vPickedPos.y)) // Scroll의 Top이 Visible 영역의 Top의 범위를 넘어가려 할 때
            {
                SetUIPos(m_pTransformCom->GetState(CTransform::STATE_POSITION).x, m_pTransformCom->GetState(CTransform::STATE_POSITION).y - (dist * 0.1f));
                /*if ((m_fY + (m_fSizeY / 2.f)) <= (m_vVisibleCenter.y + (m_fVisibleYSize / 2.f)))
                {
                    SetUIPos(m_pTransformCom->GetState(CTransform::STATE_POSITION).x, (m_vVisibleCenter.y + (m_fVisibleYSize / 2.f)) - (m_fSizeY / 2.f));
                }*/
                if ((m_fY - (m_fSizeY / 2.f)) <= (m_pUIBG.lock()->GetVisibleCenter().y - (m_pUIBG.lock()->GetVisibleSizeY() / 2.f)))
                {
                    //m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, (m_pUIBG.lock()->GetVisibleCenter().y + (m_fVisibleYSize / 2.f)) - (m_fParentYSize / 2.f));
                    SetUIPos(m_fX, (m_pUIBG.lock()->GetVisibleCenter().y - (m_pUIBG.lock()->GetVisibleSizeY() / 2.f)) + (m_fSizeY / 2.f));
                }

                /*float distRatio = (m_fY - (m_fSizeY / 2.f)) / (m_pUIBG.lock()->GetVisibleCenter().y - (m_fVisibleYSize / 2.f));

                m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, m_pUIBG.lock()->GETPOS.y + (dist * 0.02f));

                float parentDistRatio = (m_pUIBG.lock()->GetUIPos().y - m_pUIBG.lock()->GetUISize().y) /
                    m_pUIBG.lock()->GetVisibleCenter().y - (m_fVisibleYSize / 2.f);*/

                m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, m_pUIBG.lock()->GETPOS.y + (dist * 0.1f));

                if ((m_pUIBG.lock()->GETPOS.y - (m_pUIBG.lock()->GetUISize().y / 2.f)) >= m_pUIBG.lock()->GetVisibleCenter().y - (m_pUIBG.lock()->GetVisibleSizeY() / 2.f))
                {
                    m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, (m_pUIBG.lock()->GetVisibleCenter().y - (m_pUIBG.lock()->GetVisibleSizeY() / 2.f)) + (m_pUIBG.lock()->GetUISize().y / 2.f));
                }

                /*if (parentDistRatio >= distRatio)
                {
                    m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, (m_pUIBG.lock()->GETPOS.y - (m_pUIBG.lock()->GetUISize().y / 2.f)) +
                        ((m_pUIBG.lock()->GetUISize().y / 2.f) * distRatio));
                }*/
            }

            else if ((newMousePos.y > m_vPickedPos.y)) // Scroll의 Bottom이 Visible 영역의 Bottom의 범위를 넘어가려 할 때
            {
                SetUIPos(m_pTransformCom->GetState(CTransform::STATE_POSITION).x, m_pTransformCom->GetState(CTransform::STATE_POSITION).y + (dist * 0.1f));
                /*if ((m_fY - (m_fSizeY / 2.f)) >= (m_vVisibleCenter.y - (m_fVisibleYSize / 2.f)))
                {
                    SetUIPos(m_pTransformCom->GetState(CTransform::STATE_POSITION).x, (m_vVisibleCenter.y - (m_fVisibleYSize / 2.f)) + (m_fSizeY / 2.f));
                }*/

                if ((m_fY + (m_fSizeY / 2.f)) >= (m_pUIBG.lock()->GetVisibleCenter().y + (m_pUIBG.lock()->GetVisibleSizeY() / 2.f)))
                {
                    //m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, (m_pUIBG.lock()->GetVisibleCenter().y - (m_fVisibleYSize / 2.f)) + (m_fParentYSize / 2.f));
                    SetUIPos(m_fX, (m_pUIBG.lock()->GetVisibleCenter().y + (m_pUIBG.lock()->GetVisibleSizeY() / 2.f)) - (m_fSizeY / 2.f));
                }

                m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, m_pUIBG.lock()->GETPOS.y - (dist * 0.1f));

                if (m_pUIBG.lock()->GETPOS.y + (m_pUIBG.lock()->GetUISize().y / 2.f) <= m_pUIBG.lock()->GetVisibleCenter().y + (m_pUIBG.lock()->GetVisibleSizeY() / 2.f))
                {
                    m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, (m_pUIBG.lock()->GetVisibleCenter().y + (m_pUIBG.lock()->GetVisibleSizeY() / 2.f)) - (m_pUIBG.lock()->GetUISize().y / 2.f));
                }

                /*float distRatio = (m_fY + (m_fSizeY / 2.f)) / (m_pUIBG.lock()->GetVisibleCenter().y + (m_fVisibleYSize / 2.f));

                m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, m_pUIBG.lock()->GETPOS.y - (dist * 0.02f));

                float parentDistRatio = m_pUIBG.lock()->GetVisibleCenter().y + (m_fVisibleYSize / 2.f) /
                    (m_pUIBG.lock()->GetUIPos().y + m_pUIBG.lock()->GetUISize().y);

                if (parentDistRatio >= distRatio)
                {
                    m_pUIBG.lock()->SetUIPos(m_pUIBG.lock()->GETPOS.x, (m_pUIBG.lock()->GETPOS.y + (m_pUIBG.lock()->GetUISize().y / 2.f)) -
                        ((m_pUIBG.lock()->GetUISize().y / 2.f) * distRatio));
                }*/
            }
        }

        if (GAMEINSTANCE->MouseUp(DIM_RB))
        {
            m_isSelected = false;
            m_isScrolling = false;
        }
    }

    /*else
    {
        if (GAMEINSTANCE->MouseDown(DIM_LB))
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
        }
    }*/
}

bool CToolUIScroll::IsScrollMouseOn()
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

    /*POINT mousePt;
    ::GetCursorPos(&mousePt);
    ::ScreenToClient(g_hWnd, &mousePt);

    _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
    _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

    if ((mousePosX >= (m_BindWorldMat._41 - m_fSizeX * 0.5f)) && (mousePosX <= (m_BindWorldMat._41 + m_fSizeX * 0.5f)) && (mousePosY >= (m_BindWorldMat._42 - m_fSizeY * 0.5f) &&
        mousePosY <= m_BindWorldMat._42 + m_fSizeY * 0.5f))
        return true;

    return false;*/
}

HRESULT CToolUIScroll::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CVIInstancePoint> pBufferCom = dynamic_pointer_cast<CVIInstancePoint>(GAMEINSTANCE->GetBuffer("Buffer_InstancePoint"));
    if (pBufferCom == nullptr)
        return E_FAIL;

    m_pBuffer = dynamic_pointer_cast<CVIInstancePoint>(pBufferCom->Clone(1, &_InstanceDesc));
    if (m_pBuffer == nullptr)
        return E_FAIL;

    m_strTexKeys = _strTextureTags;

    if (m_ToolUIDesc.m_isMasked)
        m_strMaskKey = _strMaskTextureTag;

    if (m_ToolUIDesc.isNoised)
        m_strNoiseKey = _strNoiseTextureTag;

    return S_OK;
}

HRESULT CToolUIScroll::PrepareVisibleUI(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    //shared_ptr<CToolUIVisible> toolUIVsible = CToolUIVisible::Create(_strTextureTags, _strMaskTextureTag, _ToolUIDesc, _InstanceDesc);

    return S_OK;
}

shared_ptr<CToolUIScroll> CToolUIScroll::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    shared_ptr<CToolUIScroll> pInstance = make_shared<CToolUIScroll>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CToolUIScroll::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
