#include "Message.h"

CMessage::CMessage()
{
}

HRESULT CMessage::Initialize(_uint _iChestNum)
{
    return S_OK;
}

void CMessage::PriorityTick(_float _fTimeDelta)
{
}

void CMessage::Tick(_float _fTimeDelta)
{
}

void CMessage::LateTick(_float _fTimeDelta)
{
}

HRESULT CMessage::Render()
{
    return S_OK;
}

shared_ptr<CMessage> CMessage::Create(_uint _iChestNum)
{
    shared_ptr<CMessage> pMessage = make_shared<CMessage>();

    if (FAILED(pMessage->Initialize(_iChestNum)))
        MSG_BOX("Failed to Create : CMessage");

    return pMessage;
}
