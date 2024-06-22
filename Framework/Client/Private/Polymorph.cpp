#include "Polymorph.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"
#include "CameraMgr.h"
#include "CameraSpherical.h"

CPolymorph::CPolymorph()
{
}

CPolymorph::~CPolymorph()
{
}

void CPolymorph::EnableCollision(const char* _strShapeTag)
{
	m_pRigidBody->EnableCollision(_strShapeTag);
}

void CPolymorph::DisableCollision(const char* _strShapeTag)
{
	m_pRigidBody->DisableCollision(_strShapeTag);
}

void CPolymorph::SetTransStart(_float _fChangingTime)
{
	m_fChangingTime = _fChangingTime;
}

void CPolymorph::UpdateLightDesc()
{


	if (m_IsDungeonLevel) {

		/*Move Player Light*/
		_float3 vCurrentPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
		LIGHT_DESC* pLightDesc = GAMEINSTANCE->GetLightDesc(m_iLightIdxFront);
		LIGHT_DESC* pLightDesc2 = GAMEINSTANCE->GetLightDesc(m_iLightIdxBack);

		if (pLightDesc)
		{
			pLightDesc->vPosition = _float4(vCurrentPos.x, vCurrentPos.y + 1.1f, vCurrentPos.z - 0.2f, 1.f);
			pLightDesc2->vPosition = _float4(vCurrentPos.x, vCurrentPos.y + 1.1f, vCurrentPos.z + 0.2f, 1.f);
		}

	}


}
