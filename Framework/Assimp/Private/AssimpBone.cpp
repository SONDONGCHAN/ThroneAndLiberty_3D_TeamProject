#include "AssimpBone.h"

#include <fstream>

CAssimpBone::CAssimpBone()
{
}

CAssimpBone::~CAssimpBone()
{
}

HRESULT CAssimpBone::Initialize(const aiNode* _pAIBone, _int _iParentBoneIndex)
{
    strcpy_s(m_szName, _pAIBone->mName.data);

    //AIscene에서 바로 읽어오면 col-major Matrix로 읽어지기 때문에 전치해주는 과정이 필요하다
    memcpy(&m_TransformationMatrix, &_pAIBone->mTransformation, sizeof(_float4x4));

    if (-1 != m_iParentBoneIndex)
        XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    else
        XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

    //초기에는 값을 세팅할 수 없기에 identity를 넣어둔다
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    m_iParentBoneIndex = _iParentBoneIndex;

    return S_OK;
}

void CAssimpBone::ParsingBone(ofstream& out)
{
    _ulong dwByte = 0;

    out.write((char*)m_szName, MAX_PATH * sizeof(char));
    out.write((char*)&m_iParentBoneIndex, sizeof(_int));
    out.write((char*)&m_TransformationMatrix, sizeof(_float4x4));


}

shared_ptr<CAssimpBone> CAssimpBone::Create(const aiNode* _pAIBone, _int _iParentBoneIndex)
{
    shared_ptr<CAssimpBone> pinstance = make_shared<CAssimpBone>();

    pinstance->Initialize(_pAIBone, _iParentBoneIndex);

    return pinstance;
}
