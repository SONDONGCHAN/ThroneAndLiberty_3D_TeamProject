#include "ShaderHandler.h"
#include "Shader.h"

#include "GameInstance.h"
#include "Model.h"
#include "InstancingModel.h"
#include "VIInstanceMesh.h"

#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

#include "VIRect.h"
#include "VICube.h"

#include "VIPoint.h"



CShaderHandler::CShaderHandler(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice),
    m_pContext(_pContext)
{
}

HRESULT CShaderHandler::Initialize(_float _fScreenX, _float _fScreenY)
{

    m_vScreenSize = { _fScreenX, _fScreenY };

    CreateBuffers();

    /*컴퓨트 쉐이더에서 input/ output으로 사용할 자원을 생성*/
    ReadyComputeShader();

    /*Client Path*/
    ReadShaderFile(m_strClientShaderPath);

    return S_OK;
}

HRESULT CShaderHandler::ReadyComputeShader()
{

    _float* pData = new _float();

    /*Structure Buffer*/
    D3D11_BUFFER_DESC BufferDesc;
    ZeroMemory(&BufferDesc, sizeof(BufferDesc));

    BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    BufferDesc.ByteWidth = sizeof(_float);
    BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    BufferDesc.StructureByteStride = sizeof(_float);

    D3D11_SUBRESOURCE_DATA SubResource;
    SubResource.pSysMem = pData;
    SubResource.SysMemPitch = 0;
    SubResource.SysMemSlicePitch = 0;

    wrl::ComPtr<ID3D11Buffer> pBuffer;

    /*SRV의 Base가 될 Buffer 생성*/
    m_pDevice->CreateBuffer(&BufferDesc, &SubResource, pBuffer.GetAddressOf());

    /*Compute Shader용 SRV 설정 : Input Bounding*/
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc; 
    ZeroMemory(&SRVDesc, sizeof(SRVDesc));
    SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    SRVDesc.Buffer.ElementWidth = 1;

    /*SRV 생성*/
    m_pDevice->CreateShaderResourceView(pBuffer.Get(), &SRVDesc, m_pSRV.GetAddressOf());

    /*Compute Shader용 UAV 설정 : Output Rebounding*/
    D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
    ZeroMemory(&UAVDesc, sizeof(UAVDesc));

    UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
    UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    UAVDesc.Buffer.Flags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    UAVDesc.Buffer.NumElements = 0;

    /*UAV 생성*/
    m_pDevice->CreateUnorderedAccessView(pBuffer.Get(), &UAVDesc, m_pUAV.GetAddressOf());

    delete pData;

    return S_OK;
}

void CShaderHandler::ReadShaderFile(const wstring& _strBasePath)
{
    shared_ptr<CShader> pShader = nullptr;

    filesystem::path basePath(m_strEngineShaderPath);

    for (const auto& entry : filesystem::directory_iterator(basePath)) {
        if (entry.is_regular_file()) {
            if (".hlsl" == entry.path().extension()) {
                wstring strFileName = entry.path().filename().wstring();
                ClassificationShaderFile(strFileName, entry.path());
            }
        }
    }

}

void CShaderHandler::ClassificationShaderFile(const wstring& _strFileName, const wstring& _strTotalPath)
{
    shared_ptr<CShader> pShader;

    /*디퍼드*/
    if (wstring::npos != _strFileName.find(TEXT("Deferred"))) {

        pShader = pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
        m_Shaders[ST_DEFERRED] = pShader;
        m_Buffers[ST_DEFERRED] = m_BufferList["VIRect"];

    }
    /*VIRect : Position + Texcoord */
    else if (wstring::npos != _strFileName.find(TEXT("PosTex"))) {

        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
        m_Shaders[ST_POSTEX] = pShader;
        m_Buffers[ST_POSTEX] = m_BufferList["VIRect"];

    }
    else if (wstring::npos != _strFileName.find(TEXT("Shader_VtxMeshInstance"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXMESH_INSTANCE::Elements, VTXMESH_INSTANCE::iNumElements);
        m_Shaders[ST_MESHINSTANCE] = pShader;
    }
    /*NonAnimModel */
    else if (wstring::npos != _strFileName.find(TEXT("VtxMesh"))) {

        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXMESH::Elements, VTXMESH::iNumElements);
        m_Shaders[ST_NONANIM] = pShader;
    }
    /*AnimModel */
    else if (wstring::npos != _strFileName.find(TEXT("Anim"))) {

        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXANIMMESH::Elements, VTXANIMMESH::iNumElements);
        m_Shaders[ST_ANIM] = pShader;
    }
    /*Normal Texture*/
    else if (wstring::npos != _strFileName.find(TEXT("NorTex"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXNORTEX::Elements, VTXNORTEX::iNumElements);
        m_Shaders[ST_NORTEX] = pShader;
    }
    /* EffectMesh */
    else if (wstring::npos != _strFileName.find(TEXT("EffectMesh"))) {

        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXMESH::Elements, VTXMESH::iNumElements);
        m_Shaders[ST_EFFECTMESH] = pShader;
    }
    /* EffectTex*/
    else if (wstring::npos != _strFileName.find(TEXT("EffectTex"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
        m_Shaders[ST_EFFECTTEX] = pShader;
    }
    /* Particle */
    else if (wstring::npos != _strFileName.find(TEXT("Shader_VtxPointInstance"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPARTICLE_INSTANCE::Elements, VTXPARTICLE_INSTANCE::iNumElements);
        m_Shaders[ST_PARTICLE] = pShader;
    }
    /* Trail */
    else if (wstring::npos != _strFileName.find(TEXT("Trail"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
        m_Shaders[ST_TRAIL] = pShader;
    }
    /* UI Instance */
    else if (wstring::npos != _strFileName.find(TEXT("UIInstance")))
    {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXUIINSTANCE::Elements, VTXUIINSTANCE::iNumElements);
        m_Shaders[ST_UIINSTANCE] = pShader;
    }
    /* UI */
    else if (wstring::npos != _strFileName.find(TEXT("UI"))) {

        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOINT_INSTANCE::Elements, VTXPOINT_INSTANCE::iNumElements);
        m_Shaders[ST_UI] = pShader;
    }
    /*HDR : 테스트중*/
    else if (wstring::npos != _strFileName.find(TEXT("HDR"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
        m_Shaders[ST_HDR] = pShader;
        m_Buffers[ST_HDR] = m_BufferList["VIRect"];

    }
    else if (wstring::npos != _strFileName.find(TEXT("Cube"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXCUBE::Elements, VTXCUBE::iNumElements);
        m_Shaders[ST_CUBE] = pShader;
    }
    else if (wstring::npos != _strFileName.find(TEXT("Post"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
        m_Shaders[ST_POSTPROCESS] = pShader;
        m_Buffers[ST_POSTPROCESS] = m_BufferList["VIRect"];
    }
    else if (wstring::npos != _strFileName.find(TEXT("Fxaa"))) {
        pShader = pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
        m_Shaders[ST_FXAA] = pShader;
        m_Buffers[ST_FXAA] = m_BufferList["VIRect"];
    }
    else if (wstring::npos != _strFileName.find(TEXT("DOF"))) {
        pShader = pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
        m_Shaders[ST_DOF] = pShader;
        m_Buffers[ST_DOF] = m_BufferList["VIRect"];
    }
    /*Compute Shader*/
    else if (wstring::npos != _strFileName.find(TEXT("Compute"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
        m_Shaders[ST_COMPUTE] = pShader;
    }
    else if (wstring::npos != _strFileName.find(TEXT("Shader_VtxPoint"))) {
        pShader = CShader::Create(m_pDevice, m_pContext, _strTotalPath, VTXPOS::Elements, VTXPOS::iNumElements);
        m_Shaders[ST_POINT] = pShader;
        m_Buffers[ST_POINT] = m_BufferList["VIPoint"];
    }

    
    /*범용적으로 사용하는 고정 인자(ScreenSize, Far) 같은 변수를 초기에 미리 Bind 해둔다 */
    if(pShader)
        pShader->BindBaseValue(m_vScreenSize.x, m_vScreenSize.y, 1000.f);

}

void CShaderHandler::CreateBuffers()
{
    shared_ptr<CVIBuffer> pVIRect = CVIRect::Create(m_pDevice, m_pContext);
    m_BufferList.emplace("VIRect", pVIRect);

    shared_ptr<CVIBuffer> pVICube = CVICube::Create(m_pDevice, m_pContext);
    m_Buffers[ST_CUBE] = pVICube;

    shared_ptr<CVIBuffer> pVIPoint = CVIPoint::Create(m_pDevice, m_pContext);
    m_BufferList.emplace("VIPoint", pVIPoint);
}

void CShaderHandler::ModelMaterialAutoBind(shared_ptr<class CModel> _pModel, _uint _iMeshNum)
{
    vector<shared_ptr<CMesh>> Meshes = _pModel->GetMeshes();
    vector<shared_ptr<CMaterial>>* Materials = _pModel->GetMaterial();

    _uint iMaterialIndex = Meshes[_iMeshNum]->GetMaterialIndex();

    unordered_map<_uint, string> SRVs = (*Materials)[iMaterialIndex]->GetTextures();
    /*NormalTexture*/
    auto iter = SRVs.find(ETEXTURE_TYPE::NORMALS);


    _bool bUsingTex = false;

    if (FAILED(_pModel->BindMaterialShaderResource((_uint)_iMeshNum, ETEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture")))
        return;

    /*텍스쳐 있음*/
    if (iter != SRVs.end()) {
        
        bUsingTex = true;

        if (FAILED(m_pCurrentShader->BindRawValue("g_IsUsingNormal", &bUsingTex, sizeof(_bool))))
            return;

        if (FAILED(_pModel->BindMaterialShaderResource((_uint)_iMeshNum, ETEXTURE_TYPE::NORMALS, "g_NormalTexture")))
            return;

    }
    /*텍스쳐 없음*/
    else {

        if (FAILED(m_pCurrentShader->BindRawValue("g_IsUsingNormal", &bUsingTex, sizeof(_bool))))
            return;

    }



    /*ARMTexture*/
    iter = SRVs.find(ETEXTURE_TYPE::METALNESS);


    bUsingTex = false;

    /*텍스쳐 있음*/
    if (iter != SRVs.end()) {

        bUsingTex = true;

        if (FAILED(m_pCurrentShader->BindRawValue("g_IsUsingARM", &bUsingTex, sizeof(_bool))))
            return;

        if (FAILED(_pModel->BindMaterialShaderResource((_uint)_iMeshNum, ETEXTURE_TYPE::METALNESS, "g_ARMTexture")))
            return;

    }
    /*텍스쳐 없음*/
    else {

        if (FAILED(m_pCurrentShader->BindRawValue("g_IsUsingARM", &bUsingTex, sizeof(_bool))))
            return;

    }

}

void CShaderHandler::InstanceModelMaterialAutoBind(shared_ptr<class CInstancingModel> _pModel, _uint _iMeshNum)
{
    const vector<shared_ptr<CVIInstanceMesh>>& Meshes = _pModel->GetMeshes();
    vector<shared_ptr<CMaterial>>* Materials = _pModel->GetMaterial();

    _uint iMaterialIndex = Meshes[_iMeshNum]->GetMaterialIndex();

    const unordered_map<_uint, string>& SRVs = (*Materials)[iMaterialIndex]->GetTextures();
    /*NormalTexture*/
    auto iter = SRVs.find(ETEXTURE_TYPE::NORMALS);


    _bool bUsingTex = false;

    if (FAILED(_pModel->BindMaterialShaderResource((_uint)_iMeshNum, ETEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture")))
        return;

    /*텍스쳐 있음*/
    if (iter != SRVs.end()) {

        bUsingTex = true;

        if (FAILED(m_pCurrentShader->BindRawValue("g_IsUsingNormal", &bUsingTex, sizeof(_bool))))
            return;

        if (FAILED(_pModel->BindMaterialShaderResource((_uint)_iMeshNum, ETEXTURE_TYPE::NORMALS, "g_NormalTexture")))
            return;

    }
    /*텍스쳐 없음*/
    else {

        if (FAILED(m_pCurrentShader->BindRawValue("g_IsUsingNormal", &bUsingTex, sizeof(_bool))))
            return;

    }



    /*ARMTexture*/
    iter = SRVs.find(ETEXTURE_TYPE::METALNESS);


    bUsingTex = false;

    /*텍스쳐 있음*/
    if (iter != SRVs.end()) {

        bUsingTex = true;

        if (FAILED(m_pCurrentShader->BindRawValue("g_IsUsingARM", &bUsingTex, sizeof(_bool))))
            return;

        if (FAILED(_pModel->BindMaterialShaderResource((_uint)_iMeshNum, ETEXTURE_TYPE::METALNESS, "g_ARMTexture")))
            return;

    }
    /*텍스쳐 없음*/
    else {

        if (FAILED(m_pCurrentShader->BindRawValue("g_IsUsingARM", &bUsingTex, sizeof(_bool))))
            return;

    }
}

HRESULT CShaderHandler::CreateCubeMap()
{

    /*Ambient Lighting에 사용할 Cube 생성*/
    /*미리 생성한 이후 텍스쳐만 교환해가며 쉐이더에 바인드해줄 예정*/
    wrl::ComPtr<ID3D11Texture2D> pEnvMap = nullptr;

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = 256;
    desc.Height = 256;
    desc.MipLevels = 9;
    desc.ArraySize = 6;
    desc.SampleDesc.Quality = 0;
    desc.SampleDesc.Count = 1;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


    if (FAILED(m_pDevice->CreateTexture2D(&desc, NULL, pEnvMap.GetAddressOf())))
        return E_FAIL;


    D3D11_RENDER_TARGET_VIEW_DESC descRT;
    descRT.Format = desc.Format;
    descRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    descRT.Texture2DArray.ArraySize = 1;
    descRT.Texture2DArray.MipSlice = 0;

    D3D11_BOX sourceRegion;

    ComPtr<ID3D11ShaderResourceView> pRes =  GAMEINSTANCE->GetSRV("Side_0");

    for (_uint i = 0; i < desc.ArraySize; ++i) {

        sourceRegion.left = 0;
        sourceRegion.right = 256;
        sourceRegion.top = 0;
        sourceRegion.bottom = 256;
        sourceRegion.front = 0;
        sourceRegion.back = 1;

        m_pContext->CopySubresourceRegion(pEnvMap.Get(), D3D11CalcSubresource(0, i, 9), 0, 0, 0, (ID3D11Resource*)pRes.Get(),
            0, &sourceRegion);
    }



    D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
    descSRV.Format = desc.Format;
    descSRV.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
    descSRV.TextureCube.MipLevels = 9;
    descSRV.TextureCube.MostDetailedMip = 0;


    if (FAILED(m_pDevice->CreateShaderResourceView(pEnvMap.Get(), &descSRV, m_pHDRCubeSRV.GetAddressOf())))
        return E_FAIL;


    return S_OK;
}

HRESULT CShaderHandler::ReadyShader(ESHADER_TYPE _eShderType, _uint iPassIndex, _uint iTechniqueIndex)
{
    if (_eShderType >= ESHADER_TYPE::ST_END)
        return E_FAIL;
    
    m_pCurrentShader = m_Shaders[_eShderType];
    m_iCurrentPassIdx = iPassIndex;
    m_iCurrentTechniqueIdx = iTechniqueIndex;
    m_eCurrentShaderType = _eShderType;

    return S_OK;
}

HRESULT CShaderHandler::BeginShader()
{
    if (ESHADER_TYPE::ST_NONANIM == m_eCurrentShaderType ||
        ESHADER_TYPE::ST_ANIM == m_eCurrentShaderType ||
        ESHADER_TYPE::ST_EFFECTMESH == m_eCurrentShaderType ||
        ESHADER_TYPE::ST_TRAIL == m_eCurrentShaderType ||
        ESHADER_TYPE::ST_END == m_eCurrentShaderType)
        return E_FAIL;

    /* For UI - Added By Gwan */
    if (ESHADER_TYPE::ST_UI == m_eCurrentShaderType
        || ESHADER_TYPE::ST_PARTICLE == m_eCurrentShaderType
        || ESHADER_TYPE::ST_UIINSTANCE == m_eCurrentShaderType)
    {
        m_pCurrentShader->Begin(m_iCurrentPassIdx, m_iCurrentTechniqueIdx);

        return S_OK;
    }

    m_pCurrentShader->Begin(m_iCurrentPassIdx, m_iCurrentTechniqueIdx);
    m_Buffers[m_eCurrentShaderType]->BindBuffers();
    m_Buffers[m_eCurrentShaderType]->Render();

    return S_OK;

}

HRESULT CShaderHandler::BeginCS(_uint _iThreadX, _uint _iThreadY, _uint _iThreadZ)
{

    m_pCurrentShader->Begin(m_iCurrentPassIdx, m_iCurrentTechniqueIdx);
    m_pContext->Dispatch(_iThreadX, _iThreadY, 1);


    return S_OK;
}

HRESULT CShaderHandler::BeginAnimModel(shared_ptr<class CModel> _pModel, _uint _iMeshNum)
{   
    /*Texture Binding*/
    ModelMaterialAutoBind(_pModel, _iMeshNum);

    /*Render*/
    if (FAILED(_pModel->BindBoneMatrices(m_pCurrentShader, "g_BoneMatrices", _iMeshNum)))
        return E_FAIL;

    if (FAILED(m_pCurrentShader->Begin(m_iCurrentPassIdx)))
        return E_FAIL;

    if (FAILED(_pModel->Render((_uint)_iMeshNum)))
        return E_FAIL;


    return S_OK;
}

HRESULT CShaderHandler::BeginAnimModelBone(shared_ptr<class CModel> _pModel, _float4x4* _BoneMatrices, _uint _iMeshNum)
{
    /*Texture Binding*/
    ModelMaterialAutoBind(_pModel, _iMeshNum);
    _float4x4		BoneMatrices[MAX_BONE];

    /*Render*/
    if (FAILED(m_pCurrentShader->BindMatrices("g_BoneMatrices", _BoneMatrices, MAX_BONE)))
        return E_FAIL;

    if (FAILED(m_pCurrentShader->Begin(m_iCurrentPassIdx)))
        return E_FAIL;

    if (FAILED(_pModel->Render((_uint)_iMeshNum)))
        return E_FAIL;


    return S_OK;
}

HRESULT CShaderHandler::BeginNonAnimModel(shared_ptr<class CModel> _pModel, _uint _iMeshNum)
{
    /*Texture Binding*/
    ModelMaterialAutoBind(_pModel, _iMeshNum);

    /*Render*/

    if (FAILED(m_pCurrentShader->Begin(m_iCurrentPassIdx)))
        return E_FAIL;

    if (FAILED(_pModel->Render((_uint)_iMeshNum)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderHandler::BeginNonAnimInstance(shared_ptr<class CInstancingModel> _pModel, _uint _iMeshNum)
{
    /*Texture Binding*/
    InstanceModelMaterialAutoBind(_pModel, _iMeshNum);

    /*Render*/

    if (FAILED(m_pCurrentShader->Begin(m_iCurrentPassIdx)))
        return E_FAIL;

    if (FAILED(_pModel->Render((_uint)_iMeshNum)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderHandler::BeginShaderBuffer(shared_ptr<class CVIBuffer> _pVIBuffer)
{
    m_pCurrentShader->Begin(m_iCurrentPassIdx, m_iCurrentTechniqueIdx);
    _pVIBuffer->BindBuffers();
    _pVIBuffer->Render();

    return S_OK;
}

HRESULT CShaderHandler::BindWVPMatrixOrthographic(_float4x4 _worldMat)
{

    /*Set World*/
    if (FAILED(m_pCurrentShader->BindMatrix("g_WorldMatrix", &_worldMat)))
        return E_FAIL;

    /*Set ViewMatrix*/
    _float4x4 ViewMat = XMMatrixIdentity();

    if (FAILED(m_pCurrentShader->BindMatrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;

    /*Set ProjMatrix*/
    _float4x4 ProjMat = XMMatrixOrthographicLH(m_vScreenSize.x, m_vScreenSize.y, 0.f, 1.f);

    if (FAILED(m_pCurrentShader->BindMatrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderHandler::BindWVPMatrixPerspective(_float4x4 _worldMat)
{
    /*Set World*/
    if (FAILED(m_pCurrentShader->BindMatrix("g_WorldMatrix", &_worldMat)))
        return E_FAIL;

    /*Set ViewMatrix*/
    _float4x4 ViewMat = CGameInstance::GetInstance()->GetTransformMatrix(CPipeLine::D3DTS_VIEW);

    if (FAILED(m_pCurrentShader->BindMatrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;

    /*Set ProjMatrix*/
    _float4x4 ProjMat = CGameInstance::GetInstance()->GetTransformMatrix(CPipeLine::D3DTS_PROJ);

    if (FAILED(m_pCurrentShader->BindMatrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderHandler::BindRawValue(const _char* _pConstantName, const void* _pData, _uint _iLength)
{
    return m_pCurrentShader->BindRawValue(_pConstantName, _pData, _iLength);
}

HRESULT CShaderHandler::BindMatrix(const _char* _pConstantName, const _float4x4* _pMatrix)
{
    return m_pCurrentShader->BindMatrix(_pConstantName, _pMatrix);
}

HRESULT CShaderHandler::BindMatrices(const _char* _pConstantName, const _float4x4* _pMatrices, _uint _iNumMatrices)
{
    return m_pCurrentShader->BindMatrices(_pConstantName, _pMatrices, _iNumMatrices);
}

HRESULT CShaderHandler::BindSRV(const _char* _pConstantName, string _strTexTag)
{
    auto pTex = GAMEINSTANCE->GetSRV(_strTexTag);

    /*키값에 맞는 SRV를 반납 받는다*/
    if (!pTex)
        return E_FAIL;

    return m_pCurrentShader->BindSRV(_pConstantName, pTex);
}

HRESULT CShaderHandler::BindSRVDirect(const _char* _pConstantName, wrl::ComPtr<ID3D11ShaderResourceView> _pSRV)
{
    return m_pCurrentShader->BindSRV(_pConstantName, _pSRV);
}

HRESULT CShaderHandler::BindFloatVectorArray(const _char* _pConstantName, const _float4* _pVectors, _uint _iNumVectors)
{
	return m_pCurrentShader->BindFloatVectorArray(_pConstantName, _pVectors, _iNumVectors);
}

HRESULT CShaderHandler::BindUAV(const _char* _pConstantName, wrl::ComPtr<ID3D11UnorderedAccessView> _pUAV)
{
    return m_pCurrentShader->BindUAV(_pConstantName, _pUAV);
}

HRESULT CShaderHandler::BindHDRCube()
{

    return m_pCurrentShader->BindSRV("g_irradianceCube", m_pHDRCubeSRV);

}

HRESULT CShaderHandler::BindLightProjMatrix()
{
    if(FAILED(m_pCurrentShader->BindMatrices("g_CascadeProjMat", GAMEINSTANCE->GetShadowProjMat(), 3)))
        return E_FAIL;

    if (FAILED(m_pCurrentShader->BindMatrices("g_CascadeViewMat", GAMEINSTANCE->GetShadowViewMat(), 3)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShaderHandler::BindLightVPMatrix()
{
    _float4x4		ViewMatrix, ProjMatrix;

    XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
    XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), (_float)1280.0f / 720.0f, 0.1f, 2000.f));

    if (FAILED(m_pCurrentShader->BindMatrix("g_LightViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pCurrentShader->BindMatrix("g_LightProjMatrix", &ProjMatrix)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CShaderHandler> CShaderHandler::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, _float _fScreenX, _float _fScreenY)
{
    shared_ptr<CShaderHandler> pInstacne = make_shared<CShaderHandler>(_pDevice, _pContext);

    if (FAILED(pInstacne->Initialize(_fScreenX, _fScreenY)))
        MSG_BOX("Faile to Create : CShaderHandler");
    
    return pInstacne;
}
