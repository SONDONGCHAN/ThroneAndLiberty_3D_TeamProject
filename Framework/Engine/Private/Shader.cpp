#include "Shader.h"

CShader::CShader(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice),
    m_pContext(_pContext)
{
}

HRESULT CShader::Initialize(const wstring& _strShaderPath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    _uint iHlslFlag = { 0 };

#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1; // =0
#endif

    if (FAILED(D3DX11CompileEffectFromFile(_strShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice.Get(), &m_pEffect, nullptr)))
        return E_FAIL;

    D3DX11_EFFECT_DESC EffectDesc = {};
    m_pEffect->GetDesc(&EffectDesc);

    for (size_t i = 0; i < EffectDesc.Techniques; i++) {

        wrl::ComPtr<ID3DX11EffectTechnique> pTechnique = m_pEffect->GetTechniqueByIndex((uint32_t)i);
        if (!pTechnique)
            return E_FAIL;

        D3DX11_TECHNIQUE_DESC TechniqueDesc = {};
        pTechnique->GetDesc(&TechniqueDesc);

        for (size_t j = 0; j < TechniqueDesc.Passes; j++) {

            wrl::ComPtr<ID3D11InputLayout> pInputLayout = { nullptr };
            wrl::ComPtr<ID3DX11EffectPass> pPass = pTechnique->GetPassByIndex((uint32_t)i);
            if (!pPass)
                return E_FAIL;

            D3DX11_PASS_DESC PassDesc = {};
            pPass->GetDesc(&PassDesc);

            if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
                return E_FAIL;

            m_InputLayouts.push_back(pInputLayout);
        }
    }

    return S_OK;
}

HRESULT CShader::BindBaseValue(_float _fScreenX, _float _fScreenY, _float _fFar)
{
    ID3DX11EffectVariable* pScreenX = m_pEffect->GetVariableByName("g_fScreenX");
    
    /*전역 변수가 존재하면 Bind*/
    if (pScreenX) {
        pScreenX->SetRawValue(&_fScreenX, 0, sizeof(_float));
    }

    ID3DX11EffectVariable* pScreenY = m_pEffect->GetVariableByName("g_fScreenY");

    if (pScreenY) {
        pScreenY->SetRawValue(&_fScreenY, 0, sizeof(_float));
    }


    //Far
    ID3DX11EffectVariable* pFar = m_pEffect->GetVariableByName("g_fFar");

    if (pFar) {
        pFar->SetRawValue(&_fFar, 0, sizeof(_float));
    }

    return S_OK;

}

HRESULT CShader::Begin(_uint iPassIndex, _uint iTechniqueIndex)
{
    wrl::ComPtr<ID3DX11EffectTechnique> pTechnique = m_pEffect->GetTechniqueByIndex(iTechniqueIndex);
    if (!pTechnique)
        return E_FAIL;

    wrl::ComPtr<ID3DX11EffectPass> pPass = pTechnique->GetPassByIndex(iPassIndex);
    if (!pPass)
        return E_FAIL;

    pPass->Apply(0, m_pContext.Get());

    m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex].Get());
    return S_OK;
}

HRESULT CShader::BindRawValue(const _char* _pConstantName, const void* _pData, _uint _iLength)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    return pVariable->SetRawValue(_pData, 0, _iLength);
}

HRESULT CShader::BindMatrix(const _char* pConstantName, const _float4x4* pMatrix)
{
    wrl::ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (!pVariable)
        return E_FAIL;

    wrl::ComPtr<ID3DX11EffectMatrixVariable> pMatrixVariable = pVariable->AsMatrix();
    if (!pMatrixVariable)
        return E_FAIL;

    return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::BindMatrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{
    wrl::ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (!pVariable)
        return E_FAIL;

    wrl::ComPtr<ID3DX11EffectMatrixVariable> pMatrixVariable = pVariable->AsMatrix();
    if (!pMatrixVariable)
        return E_FAIL;

    return pMatrixVariable->SetMatrixArray((_float*)pMatrices, 0, iNumMatrices);
}

HRESULT CShader::BindSRV(const _char* pConstantName, wrl::ComPtr<ID3D11ShaderResourceView> _pSRV)
{
    wrl::ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);

    if (!pVariable)
        return E_FAIL;

    wrl::ComPtr<ID3DX11EffectShaderResourceVariable> pSRVariable = pVariable->AsShaderResource();

    if (!pSRVariable)
        return E_FAIL;


   return pSRVariable->SetResource(_pSRV.Get());
}

HRESULT CShader::BindSRVs(const _char* pConstantName, ID3D11ShaderResourceView** _pSRV, _uint _iNumTextures)
{
    wrl::ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);

    if (!pVariable)
        return E_FAIL;

    wrl::ComPtr<ID3DX11EffectShaderResourceVariable> pSRVariable = pVariable->AsShaderResource();

    if (!pSRVariable)
        return E_FAIL;

    return pSRVariable->SetResourceArray(_pSRV, 0, _iNumTextures);
}

HRESULT CShader::BindFloatVectorArray(const _char* _pConstantName, const _float4* _pVectors, _uint _iNumVectors)
{
    wrl::ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (!pVariable)
        return E_FAIL;

    wrl::ComPtr<ID3DX11EffectVectorVariable> pVectorVariable = pVariable->AsVector();
    if (!pVectorVariable)
        return E_FAIL;

    return pVectorVariable->SetFloatVectorArray((_float*)_pVectors, 0, _iNumVectors);
}

HRESULT CShader::BindUAV(const _char* _pConstantName, wrl::ComPtr<ID3D11UnorderedAccessView> _pUAV)
{
    wrl::ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(_pConstantName);

    if (!pVariable)
        return E_FAIL;

    wrl::ComPtr<ID3DX11EffectUnorderedAccessViewVariable> pUAVVariable = pVariable->AsUnorderedAccessView();

    if (!pUAVVariable)
        return E_FAIL;


    return pUAVVariable->SetUnorderedAccessView(_pUAV.Get());
}

shared_ptr<CShader> CShader::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const wstring& _strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElements, _uint _iNumElements)
{

    shared_ptr<CShader> pInstance = make_shared<CShader>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_strShaderFilePath, _pElements, _iNumElements)))
        MSG_BOX("Failed to Create : CShader");

    return pInstance;
}
