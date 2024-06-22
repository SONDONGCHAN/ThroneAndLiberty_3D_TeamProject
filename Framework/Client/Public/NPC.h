#pragma once

#include "InteractionObj.h"
#include "PartObject.h"

#include "QuestMgr.h"
#include "DialogueMgr.h"

BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Client)

class CUIBase;

class CNPC : public CInteractionObj
{
public:
	CNPC();
	virtual ~CNPC() = default;

public:
	virtual HRESULT Initialize(_float3 _vPosition);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();

public:
	// 동물 스포너
	void SpawnAnimal(_float3 _vPosition, _float3 _vLookPos = _float3(0.f, 0.f, 0.f));
	void		SimulationOn();
	void		SimulationOff();
	void		MovePos(_float3 _vPos);					// 특정 위치로 이동

protected:
	void		UpdateDirection();						// 플레이어 방향으로 돌리기
	void		MoveFront(_float _fTimeDelta);			// 보고있는 방향으로 이동
	_bool		IsInPlayerDegree(_float _fDegree);					// 플레이어와 npc가 보는 방향 사이의 각도가 특정 각도 안인지
	_bool		IsPlayerRight();	// 플레이어가 오른쪽에 있으면 true
	void		SetWeaponRimLight(_bool _bOn, _float3 _fRimLight =
		_float3(1.f, 0.f, 0.f), _float _fPow = 1.5f);

public:
	virtual void InteractionOn();

	_float3	GetCurrentPos() { return m_vCurrentPos; }
	void	SetCurrentPos(_float3 _vCurrentPos) { m_vCurrentPos = _vCurrentPos; }
	string	GetModelName() { return m_strModelName; }
	void	SetGenType(_int _iGenType) { m_iGenType = _iGenType; }
	// DialogOrder 결정
	_int	GetDialogOrder() { return m_iDialogOrder; }
	void	SetDialogOrder(_int _iOrder) { m_iDialogOrder = _iOrder; }
	void	SetInitLookPos(_float3 _vInitLookPos) { m_vInitLookPos = _vInitLookPos; }
	

	wstring GetDisplayName() { return m_strDisplayName; }

public: /* UI */
	_float4x4& GetCombinedBoneMat(const string& _boneName);
	void SetIsDialoguePressed(_bool _bIsDialoguePressed) { m_bIsDialoguePressed = _bIsDialoguePressed; }
	_bool IsDialoguePressed() { return m_bIsDialoguePressed; }

	void SetCurMainQuestKey(const wstring& _strQuestKey) { m_strMainQuestKey = _strQuestKey; }
	void SetCurSubQuest0Key(const wstring& _strQuestKey) { m_strSubQuest0Key = _strQuestKey; }
	void SetCurSubQuest1Key(const wstring& _strQuestKey) { m_strSubQuest1Key = _strQuestKey; }

protected: /* UI */
	HRESULT PrepareNPCBar(const wstring& _strNPCTag);
	void UpdateUIPos();

protected:
	shared_ptr<CRigidBody>	m_pRigidBody = { nullptr };				// Rigid Body
	string m_strModelName = "";										// Model 이름
	wstring m_strDisplayName = L"";

protected:
	// 파츠
	map<string, shared_ptr<CPartObject>>		m_pNPCParts;

	_float3			m_vCurrentPos = _float3(0.f, 0.f, 0.f);
	_float3			m_vFirstCreatePos = _float3(0.f, 0.f, 0.f);		// 처음 위치

	// 플레이어 위치, 방향, 거리
	_float3			m_PlayerPos = _float3(0.f, 0.f, 0.f);
	_float3			m_PlayerDir = _float3(0.f, 0.f, 0.f);
	_float			m_fDistanceXZToPlayer = 0.f;

	// 계산용 시간
	_float			m_fCalculateTime1 = 0.f;
	_float			m_fCalculateTime2 = 0.f;
	_float			m_fCalculateTime3 = 0.f;

	// 움직이는 속도
	_float			m_fSpeed = 1.f;

	// 애니메이션 재생 속도
	_float			m_fAnimSpeed = 1.f;
	// 애니메이션 한키프레임당 시간
	_float			m_fKeyFrameSpeed = 0.0413f;

	_float3			m_vInitLookPos = _float3(0.f, 0.f, 0.f);

	// GenType에 따라 다른 FSM, 퀘스트를 가지고 있다
	_int			m_iGenType = 0;

	_int			m_iDialogOrder = 0;		// 이 순서에 따라 상태가 변경

	_bool			m_bTalk[10] = { 0, };	// 말하고 있는중인지

	// 돌았는지 여부
	_bool			m_bTurnRight = false;
	_bool			m_bTurnRight130 = false;
	_bool			m_bTurnLeft = false;
	_bool			m_bTurnLeft130 = false;

	_float			m_fDegree1 = 36.f;
	_float			m_fDegree2 = 108.f;
	_float			m_fDegree3 = 179.f;

	// Spawner
protected:
	_bool m_isSpawn = false;
	_float3 m_vSpwnPos = _float3(0.f, 0.f, 0.f);
	_float3	m_vLookPos = _float3(0.f, 0.f, 0.f);	// 생성시 보는 방향
	
protected: /* UI */
	shared_ptr<CUIBase> m_pNPCBar{ nullptr };
	vector<shared_ptr<CUIBase>> m_pNPCs;

	_bool m_bIsDialoguePressed{ false };

protected:
	_bool m_IsMeetQuestDone = false;

protected:
	wstring m_strMainQuestKey{ L"" };
	wstring m_strSubQuest0Key{ L"" };
	wstring m_strSubQuest1Key{ L"" };

};

END