#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

public:
	CModel(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CModel(const shared_ptr<CModel> _pOrigin);
	virtual ~CModel();

public:
	HRESULT Initialize( const _char* pModelFilePath, _float4x4 PivotMatrix, wstring _strModelName);
	HRESULT InitializeClone();
	HRESULT Render(_uint iMeshIndex);

public:
	TYPE GetModelType() { return m_eModelType; }

public:
	_uint GetNumMeshes() const {
		return m_iNumMeshes;
	}
	_int GetBoneIndex(const _char* _pBoneName) const;

	_float4x4* GetCombinedBoneMatrixPtr(const _char* pBoneName);

	void ChangeAnim(_uint _iAnimIndex, _float _fChangingDelay = 0.1f, _bool _isLoop = false, _float _fCancleRatio = 1.f);
	void ChangeLowerAnim(_uint _iAnimIndex, _float _fChangingDelay = 0.1f, _bool _isLoop = false);

public:
	HRESULT BindMaterialShaderResource(_uint _iMeshIndex, ETEXTURE_TYPE _eMaterialType, const _char* _pConstantName);
	HRESULT BindBoneMatrices(shared_ptr<class CShader> _pShader, const _char* _pConstName, _uint _iMeshIndex);
	void PlayAnimation(_float _fTimeDelta);
	void PlayReferenceAnimation();



public:
	vector<shared_ptr<class CMesh>>		GetMeshes() { return m_Meshes; }
	vector<shared_ptr<class CAnimation>> GetAnimations() { return m_Animations; }
	vector<shared_ptr<class CMaterial>>* GetMaterial() { return &m_Materials; }
	vector<shared_ptr<class CBone>>		GetBones() { return m_Bones; }
	vector<shared_ptr<class CBone>>*	GetpBones() { return &m_Bones; }


	_float4x4* GetPrevBoneMatrix() { return m_pBoneMat; }

public:
	// 애니매이션 재생 종료 여부를 반환합니다.[ture==종료]. [애니메이션이 Loop로 재생 중이면, 무조건 true를 반환]
	_bool	GetIsFinishedAnimation(); 

	// 애니매이션 캔슬 가능 여부를 반환합니다.[ture==캔슬가능상태]. [애니메이션이 Loop로 재생 중이면, 무조건 true를 반환]
	_bool	GetCanCancelAnimation();

	// 현재 애니메이션의 진행 비율을 반환합니다.
	_float	GetCurrentAnimRatio();

	// 현재 애니메이션을 반환합니다.
	shared_ptr<class CAnimation> GetCurAnimation() { return m_Animations[m_iUpperCurrentAnimation]; }

	// 분리상태 유무를 반환합니다.
	_bool* GetSeparating() { return &m_isSeparating; };

public:
	// 애니매이션 재생을 [멈출지/재생할지] 설정합니다. [true==재생]
	void	SetIsPlayAnimation(_bool _isPlayAnimation) { m_isPlayAnimation = _isPlayAnimation; }

	// 상, 하체를 구분하여 애니메이션을 돌릴 수 있도록 준비 합니다.
	void	ReadySeparate(string _SeparateBone);

	// 참조할 뼈들의 Matrix 포인터를 저장
	void	ReadyReferenceBones(shared_ptr<CModel> _pReferenceModel);

	// Shape Disable이 예약되어있는 애니메이션이 있는지 체크하고 Disable을 시켜줍니다. 
	void	CheckDisableAnim();

private:
	_float4x4 m_PivotMatrix;
	CModel::TYPE m_eModelType = CModel::TYPE::TYPE_END;

private:
	_uint m_iNumMeshes = { 0 };

	_uint m_iNumMaterials = { 0 };
	vector<shared_ptr<class CMaterial>> m_Materials;
	vector<shared_ptr<class CBone>> m_Bones;

	vector<shared_ptr<class CMesh>> m_Meshes;

	_uint m_iNumAnimations			= { 0 };
	_uint m_iUpperCurrentAnimation	= { 0 };		// 현재 애니메이션 [ 전체 애니메이션 || 상체 애니메이션 ]
	_uint m_iLowerCurrentAnimation	= { 0 };		// 하체 애니메이션

	vector<shared_ptr<class CAnimation>> m_Animations;

private:
	shared_ptr<class CAnimation> m_CurrentAnim = nullptr;
	shared_ptr<class CAnimation> m_NextAnim = nullptr;

	_uint m_RootBoneIdx			= { 0 };
	_bool m_isPlayAnimation		= { true };			// 애니메이션 재생을 [멈출지/재생할지]
	_bool m_isFinishedAnimation	= { false };		// 애니메이션 재생이 끝났는지
	_bool m_isLoopUpper			= { false };		// 현재 애니메이션에 루프를 돌릴지
	_bool m_isLoopLower			= { false };		// 현재 애니메이션에 루프를 돌릴지

	_uint m_iNumBaseBones	= { 0 };				// 상, 하체의 분기점 까지의 Bone 갯수
	_uint m_iNumUpperBones	= { 0 };				// Root + 상체의 Bone 갯수
	_bool m_isSeparateModel	= { false };			// 상, 하체를 분리할 모델 인지
	_bool m_isSeparating	= { false };			// 상, 하체를 분리재생중 인지.

	_uint m_iReserveDisableAnimIndex	= { 0 };
	_bool m_isReserved					= { false };



private:
	_float4x4 m_pBoneMat[MAX_BONE];

private:
	HRESULT ReadyMeshes(ifstream& _ifs);
	HRESULT ReadyMaterials(ifstream& _ifs);
	HRESULT ReadyAnimations(ifstream& _ifs, wstring _strModelName);

public:
	virtual shared_ptr<CComponent> Clone(void* _pArg);

public:
	static shared_ptr<CModel> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext,  const _char* pModelFilePath, wstring _strModelName, _float4x4 PivotMatrix = XMMatrixIdentity());

};

END