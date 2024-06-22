#include "InteractionObj.h"
#include "UIMgr.h"
#include "UIPanel.h"

CInteractionObj::CInteractionObj()
{
}

HRESULT CInteractionObj::Initialize(CTransform::TRANSFORM_DESC* _pDesc)
{
    if (FAILED(__super::Initialize(_pDesc)))
        return E_FAIL;

    m_IsInteractionOn = false;
    m_pPlayer = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

    if(!m_pPlayer.lock())
        return E_FAIL;

    m_pInteractionPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("InteractionPanel"));

    if (!m_pInteractionPanel.lock())
        return E_FAIL;


    return S_OK;
}

void CInteractionObj::PriorityTick(_float _fTimeDelta)
{
}

void CInteractionObj::Tick(_float _fTimeDelta)
{
    if (m_IsInteractionDone)
        return;

    KeyInput();
}

void CInteractionObj::LateTick(_float _fTimeDelta)
{
	if (m_IsInteractionDone)
		return;

	m_IsInteractionOn = CheckDistance();
	//m_pInteractionPanel

	if (m_IsInteractionOn && m_bInteractAble && UIMGR->IsHUDOn())
	{
		_float3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);

		_float3 destPos = vPos + m_vUIDistance;
		destPos = SimpleMath::Vector3::Transform(destPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		destPos = SimpleMath::Vector3::Transform(destPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

		if (destPos.z >= 0.f && destPos.z <= 1.f)
		{
			UIMGR->ActivateInteractionUI();
            
			_float posX = (destPos.x + 1.f) * 0.5f;
			_float posY = (destPos.y - 1.f) * -0.5f;

			posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
			posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

			m_pInteractionPanel.lock()->SetIsRender(true);
			m_pInteractionPanel.lock()->SetUIPos(posX, posY);
		}
	}
}


_bool CInteractionObj::CheckDistance()
{
    if (!m_pPlayer.lock())
        return false;

    _float3 vPlrPos = m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
    _float3 vObjPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);

    float fDistance = (vObjPos - vPlrPos).Length();

    if (fDistance <= m_fInteractDistance)
        return true;

    return false;
}


void CInteractionObj::InteractionOff()
{
}

void CInteractionObj::KeyInput()
{
    if (GAMEINSTANCE->KeyDown(DIK_F)) {

        if (m_IsInteractionOn && m_bInteractAble)
            InteractionOn();
    }
}
