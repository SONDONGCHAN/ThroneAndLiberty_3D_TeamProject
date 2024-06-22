#include "Model.h"

#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "Bone.h"
#include "Texture.h"
#include "Shader.h"
#include "GameInstance.h"

#include <fstream>
#include <iostream>


CModel::CModel(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    : CComponent(_pDevice, _pContext)
{
}

CModel::CModel(const shared_ptr<CModel> _pOrigin)
    :CComponent(_pOrigin)
    , m_PivotMatrix(_pOrigin->m_PivotMatrix)
    , m_iNumMeshes(_pOrigin->m_iNumMeshes)
    , m_Meshes(_pOrigin->m_Meshes)
    , m_iNumMaterials(_pOrigin->m_iNumMaterials)
    , m_Materials(_pOrigin->m_Materials)
    , m_iNumAnimations(_pOrigin->m_iNumAnimations)
    , m_eModelType(_pOrigin->m_eModelType)
    , m_RootBoneIdx(_pOrigin->m_RootBoneIdx)
{
    for (auto& pBone : _pOrigin->m_Bones)
        m_Bones.push_back(pBone->Clone());

    for (auto& pAnim : _pOrigin->m_Animations)
        m_Animations.push_back(pAnim->Clone());

}

CModel::~CModel() 
{
}

HRESULT CModel::Initialize( const _char* pModelFilePath,  _float4x4 PivotMatrix, wstring _strModelName)
{

    std::ifstream ifs;
    ifs.open(pModelFilePath, std::ios::binary);

    _bool IsAnimModel = true;
    ifs.read((char*)&IsAnimModel, sizeof(_bool));

    if (IsAnimModel) {
        m_eModelType = TYPE::TYPE_ANIM;
    }
    else {
        m_eModelType = TYPE::TYPE_NONANIM;
    }

    XMStoreFloat4x4(&m_PivotMatrix, XMMatrixIdentity());

    if (TYPE_ANIM == m_eModelType) {

        //본갯수
        size_t iBoneNumSize;
        ifs.read((char*)&iBoneNumSize, sizeof(size_t));

        //ReadyBone
		char szName[MAX_PATH]{};
        for (_uint i = 0; i < iBoneNumSize; ++i) {

			memset(szName, 0, sizeof(char) * MAX_PATH);
            ifs.read(szName, MAX_PATH * sizeof(char));

            _int iParentBone{};
            ifs.read((char*)&iParentBone, sizeof(_int));



            shared_ptr<CBone> pBone = CBone::Create(szName, iParentBone, i, ifs);

            m_Bones.push_back(pBone);
        }
    }

    if (FAILED(ReadyMeshes(ifs)))
        return E_FAIL;

    ifs.read((char*)&m_iNumMaterials, sizeof(size_t));

    if (FAILED(ReadyMaterials(ifs)))
        return E_FAIL;

    if (TYPE_ANIM == m_eModelType) {

        if (FAILED(ReadyAnimations(ifs, _strModelName)))
            return E_FAIL;
    }

    ifs.close();

    return S_OK;
}

HRESULT CModel::InitializeClone()

{
    return S_OK;
}

HRESULT CModel::Render(_uint _iMeshIndex)
{
    m_Meshes[_iMeshIndex]->BindBuffers();
    m_Meshes[_iMeshIndex]->Render();

    return S_OK;
}

_int CModel::GetBoneIndex(const _char* _pBoneName) const
{
    _int iBoneIndex = { -1 };

    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone) {

        iBoneIndex++;

        if (!strcmp(_pBoneName, pBone->GetBoneName()))
            return true;
        return false;
        });


    if (iter == m_Bones.end())
        return -1;


    return iBoneIndex;
}

_float4x4* CModel::GetCombinedBoneMatrixPtr(const _char* pBoneName)
{
    auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<class CBone> pBone)
    {
        if (false == strcmp(pBone->GetBoneName(), pBoneName))
            return true;

        return false;
    });

    if (m_Bones.end() == iter)
        return nullptr;

    return (*iter)->GetCombindTransformationMatrixPtr();
}

void CModel::ChangeAnim(_uint _iAnimIndex, _float _fChangingDelay, _bool _isLoop, _float _fCancleRatio)
{
    if(_isLoop && (_iAnimIndex == m_iUpperCurrentAnimation))
        return;
    
    if (m_Animations[m_iUpperCurrentAnimation]->GetIsColAnim())
    {
        m_iReserveDisableAnimIndex = m_iUpperCurrentAnimation;
        m_isReserved = true;
    }

    m_iUpperCurrentAnimation = _iAnimIndex;
    m_Animations[m_iUpperCurrentAnimation]->SetBlendingInfo(_fChangingDelay, m_Bones);
    m_Animations[m_iUpperCurrentAnimation]->SetAnimCancleRatio(_fCancleRatio);
    m_isLoopUpper = _isLoop;
}

void CModel::ChangeLowerAnim(_uint _iAnimIndex, _float _fChangingDelay, _bool _isLoop)
{
    if (!m_isSeparateModel)
        return;

    m_isSeparating = true;

    if ( _isLoop && (_iAnimIndex == m_iLowerCurrentAnimation))
        return;

    m_iLowerCurrentAnimation = _iAnimIndex;
    m_Animations[m_iLowerCurrentAnimation]->SetBlendingInfo(_fChangingDelay, m_Bones, false, m_iNumBaseBones, m_iNumUpperBones);
    m_isLoopLower = _isLoop;
}

HRESULT CModel::BindMaterialShaderResource(_uint _iMeshIndex, ETEXTURE_TYPE _eMaterialType, const _char* _pConstantName)
{
    _uint iMaterialIndex = m_Meshes[_iMeshIndex]->GetMaterialIndex();
    string strTexKey = m_Materials[iMaterialIndex]->GetTextures()[_eMaterialType];

    return GAMEINSTANCE->BindSRV(_pConstantName, strTexKey);
}

HRESULT CModel::BindBoneMatrices(shared_ptr<class CShader> _pShader, const _char* _pConstName, _uint _iMeshIndex)
{
    _float4x4		BoneMatrices[MAX_BONE];

    m_Meshes[_iMeshIndex]->SetUpBoneMatrices(BoneMatrices, m_Bones);
    memcpy_s(m_pBoneMat, sizeof(m_pBoneMat), BoneMatrices, sizeof(BoneMatrices));

    return _pShader->BindMatrices(_pConstName, BoneMatrices, MAX_BONE);
}

void CModel::PlayAnimation(_float _fTimeDelta)
{
    _float fTimeDelta = _fTimeDelta;

    if (!m_isPlayAnimation)
        fTimeDelta = 0.f;

    if (!m_isSeparating) 
        m_isFinishedAnimation = m_Animations[m_iUpperCurrentAnimation]->InvalidateTransformationMatrix(fTimeDelta, m_Bones, m_isLoopUpper);

    else
    {
        m_isFinishedAnimation = m_Animations[m_iUpperCurrentAnimation]->SeparateTransformationMatrix(fTimeDelta, m_Bones, m_isLoopUpper, true, m_iNumBaseBones, m_iNumUpperBones);
        m_Animations[m_iLowerCurrentAnimation]->SeparateTransformationMatrix(fTimeDelta, m_Bones, m_isLoopLower, false, m_iNumBaseBones, m_iNumUpperBones);
    }

    if (m_isSeparating)
        m_Bones[m_iNumBaseBones]->SetOption(CBone::OPTION_SUBTRACT_ROTATION_Y);
    else
        m_Bones[m_iNumBaseBones]->SetOption(CBone::OPTION_DEFAULT);
    
    for (auto& pBone : m_Bones)
    {
        pBone->InvalidateCombinedTransformationMatrix(m_Bones);
    }
    m_isSeparating = false;
}

void CModel::PlayReferenceAnimation()
{
    for (auto& pBone : m_Bones)
    {
        pBone->InvalidateRefTransformationMatrix();
    }
    
}

_bool CModel::GetIsFinishedAnimation()
{
    if (m_isLoopUpper)
        return true;

    return m_isFinishedAnimation;
}

_bool CModel::GetCanCancelAnimation()
{
    return  m_Animations[m_iUpperCurrentAnimation]->GetCanCancel();
}

_float CModel::GetCurrentAnimRatio()
{
    return m_Animations[m_iUpperCurrentAnimation]->GetAnimRatio();
}

void CModel::ReadySeparate(string _SeparateBone)
{
    m_isSeparateModel = true;

    _uint iBoneIndex = 0;
    m_iNumBaseBones = 0;
    m_iNumUpperBones = 0;

    vector<shared_ptr<class CBone>> RootBones;
    vector<shared_ptr<class CBone>> UpperBones;
    vector<shared_ptr<class CBone>> LowerBones;

    for (auto pBone : m_Bones)
    {
        if (pBone->GetBoneName() == _SeparateBone)
        {
            UpperBones.push_back(pBone);
            break;
        }
        RootBones.push_back(pBone);

        iBoneIndex++;
    }
    
    for (_uint i = (iBoneIndex+1) ;  i < m_Bones.size() ; i++)
    {
        _int    iFinalIndex = i;
        _bool   isUpperBone = false;

        while (iFinalIndex != -1)
        {
            iFinalIndex = m_Bones[iFinalIndex]->GetParentBoneIndex();
    
            if (iFinalIndex == iBoneIndex)
            {
                UpperBones.push_back(m_Bones[i]);
                isUpperBone = true;
                break;
            }
        }  
        if(!isUpperBone)
            LowerBones.push_back(m_Bones[i]);
    }

    m_Bones.clear();
    
    for (auto pBone : RootBones)
    {
        m_Bones.push_back(pBone);
        m_iNumBaseBones++;
        m_iNumUpperBones++;
    }
    for (auto pBone : UpperBones)
    {
        m_Bones.push_back(pBone);
        m_iNumUpperBones++;
    }
    for (auto pBone : LowerBones)
    {
        m_Bones.push_back(pBone);
    }

    for (auto pBone : m_Bones)
    {
        _int iParentBoneIndex = pBone->GetParentBoneIndex();
        
        if (iParentBoneIndex == -1)
            continue;

        for (_uint i = 0; i < m_Bones.size(); i++)
        {
            if (m_Bones[i]->GetOriginIndex() == iParentBoneIndex)
            {
                pBone->SetParentBoneIndex(i);
            }
        }
    }

    for(auto pAnimation : m_Animations)
        pAnimation->UpdateChannelsBoneIndex(dynamic_pointer_cast<CModel>(shared_from_this()));


}

void CModel::ReadyReferenceBones(shared_ptr<CModel> _pReferenceModel)
{
    vector<shared_ptr<class CBone>>* pRefBones =_pReferenceModel->GetpBones();

    for (auto pBone : m_Bones)
    {
        _bool isRef = false;

        for (auto pReferenceBone : (*pRefBones))
        {
            if (false == strcmp(pBone->GetBoneName(), pReferenceBone->GetBoneName()))
            {
                pBone->SetCombindTransformationMatrixPtr(pReferenceBone->GetCombindTransformationMatrixPtr());
                isRef = true;
                //MyBones.push_back(pBone);
                break;
            }
        }

        if(!isRef)
            pBone->SetCombindTransformationMatrixPtr(nullptr);
    }

    for (auto pBone : m_Bones)
    {
        if (nullptr == (pBone->GetRefTransformationMatrixPtr()))
        {
            _int Index = pBone->GetParentBoneIndex();

            while (true)
            {
                if (m_Bones[Index]->GetRefTransformationMatrixPtr())
                {
                    pBone->SetCombindTransformationMatrixPtr(m_Bones[Index]->GetRefTransformationMatrixPtr());
                    break;
                }
                else
                {
                    if (Index == -1)
                        break;

                    Index = m_Bones[Index]->GetParentBoneIndex();
                }
            }      
        }      
    }
}

void CModel::CheckDisableAnim()
{
    if (!m_isReserved)
        return;

    m_isReserved = false;

    m_Animations[m_iReserveDisableAnimIndex]->DisableCollision();
}

 
HRESULT CModel::ReadyMeshes(ifstream& _ifs)
{
    _ifs.read((char*)&m_iNumMeshes, sizeof(size_t));

    for (size_t i = 0; i < m_iNumMeshes; i++) {

        shared_ptr<CMesh> pMesh = CMesh::Create(m_eModelType, m_pDevice, m_pContext, _ifs, XMLoadFloat4x4(&m_PivotMatrix));

        if (!pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::ReadyMaterials(ifstream& _ifs)
{
    _ulong dwByte = 0;

    for (size_t i = 0; i < m_iNumMaterials; i++) {

        map<_uint, weak_ptr<class CTexture>> Textures;

        unordered_map<_uint, string> SRVMap;

        size_t iTotTextureIdx = 0;
        _ifs.read((char*)&iTotTextureIdx, sizeof(size_t));

        for (size_t j = 0; j < iTotTextureIdx; j++) {

            char szTexName[MAX_PATH];
            _uint iTextureType;

            _ifs.read((char*)szTexName, sizeof(char) * MAX_PATH);
            _ifs.read((char*)&iTextureType, sizeof(_uint));

            SRVMap.emplace(iTextureType, szTexName);
        }

        m_Materials.push_back(CMaterial::Create(SRVMap));

    }

    return S_OK;
}

HRESULT CModel::ReadyAnimations(ifstream& _ifs, wstring _strModelName)
{
    _ifs.read((char*)&m_iNumAnimations, sizeof(size_t));

    shared_ptr<CModel> pModel = dynamic_pointer_cast<CModel>(shared_from_this());

    Json::Value Animation;

    wstring strBaseAnimDataPath = TEXT("..\\..\\Client\\Bin\\DataFiles\\AnimData\\");

    //모델명도 붙여야함!
    strBaseAnimDataPath += _strModelName + TEXT(".json");
    Animation = GAMEINSTANCE->ReadJson(strBaseAnimDataPath);

    for (size_t i = 0; i < m_iNumAnimations; i++) {

        /*애니메이션 json을 읽어서 */

        string strKeyNum = to_string(i);

        _float _fAnimSpeed = Animation[strKeyNum]["Speed"].asFloat();
        //_fAnimSpeed
        shared_ptr<CAnimation> pAnimation = CAnimation::Create(_ifs, pModel, _fAnimSpeed);

        if (!pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }

    // 사운드 Load
    wstring strModelSoundDataPath = TEXT("..\\..\\Tool_Animation\\Bin\\DataFiles\\SoundTrigger\\");
    strModelSoundDataPath += _strModelName + TEXT("_Sound") + TEXT(".json");

    filesystem::path path(strModelSoundDataPath);
    if(!(filesystem::exists(path) && filesystem::is_regular_file(path) ))
        return S_OK;
       
    Json::Value ModelSound;

    ModelSound = GAMEINSTANCE->ReadJson(strModelSoundDataPath);

    auto iter = ModelSound.begin();

    for (iter; iter != ModelSound.end(); ++iter)
    {
        string strAnimIndex = iter.key().asString();
        _uint iAnimIndex = atoi(strAnimIndex.c_str());

        m_Animations[iAnimIndex]->SetIsSoundAnim();

        for (_uint i = 0; i < ModelSound[strAnimIndex]["TrackPos"].size(); ++i)
        {
            string strSoundName = ModelSound[strAnimIndex]["SoundName"][i].asString();
            _float fTrackPos = ModelSound[strAnimIndex]["TrackPos"][i].asFloat();
            _float fVolume   = ModelSound[strAnimIndex]["Volume"][i].asFloat();

            m_Animations[iAnimIndex]->AddSound(strSoundName, fTrackPos, fVolume);
        }
    }
    return S_OK;
}

shared_ptr<CComponent> CModel::Clone(void* _pArg)
{
    shared_ptr<CComponent> pModel = make_shared<CModel>(static_pointer_cast<CModel>(shared_from_this()));

    return pModel;
}

shared_ptr<CModel> CModel::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const _char* pModelFilePath, wstring _strModelName, _float4x4 PivotMatrix)
{
    shared_ptr<CModel> pInstance = make_shared<CModel>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(pModelFilePath, PivotMatrix, _strModelName))) {
        MSG_BOX("Failed to Create: CModel");
    }

    return pInstance;
}

