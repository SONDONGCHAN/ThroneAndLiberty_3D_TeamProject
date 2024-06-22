#pragma once

/* =============================================== */
//	[ShaderHandler]
// 
//  셰이더 객체 관리 및 Render 할 때 Bind나 Begin 등의 역할을
//	수행해주는 핸들러
//
//  객체에서 셰이더 객체를 직접 들고 있는게 아닌, 셰이더 타입만 들고 있게 한다  
//  모델이 아닌 공통적으로 사용할 수 있는 Buffer(VIRect, VIPoint..)는 자동으로 렌더까지 해준다
//  모델의 경우 모델 포인터를 그대로 넘겨 이미지 바인딩을 자동으로 해주고 렌더 호출까지 해준다 
//
/* =============================================== */


#include "Engine_Defines.h"

BEGIN(Engine)

class CShaderHandler final
{

public:
	CShaderHandler(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	~CShaderHandler() = default;

public:
	HRESULT Initialize(_float _fScreenX, _float _fScreenY);

	HRESULT ReadyComputeShader();

/*Bind Shader*/
public:
	/*Bind 이전 초기 Shader 세팅*/
	HRESULT ReadyShader(ESHADER_TYPE _eShderType, _uint iPassIndex, _uint iTechniqueIndex = 0);
	
	/*모델을 사용하지 않는 객체 전용 Begin*/
	HRESULT BeginShader();

	/*For. Compute Shader*/
	HRESULT BeginCS(_uint _iThreadX, _uint _iThreadY, _uint _iThreadZ);

	/*애니메이션 모델 전용 Begin*/
	HRESULT BeginAnimModel(shared_ptr<class CModel> _pModel , _uint _iMeshNum);

	/*애니메이션 모델 & 본 정보 Begin */
	HRESULT BeginAnimModelBone(shared_ptr<class CModel> _pModel, _float4x4* _BoneMatrices, _uint _iMeshNum);

	/*논애님 모델 전용 Begin*/
	HRESULT BeginNonAnimModel(shared_ptr<class CModel> _pModel, _uint _iMeshNum);

	/*논애님 인스턴싱 모델 전용 Begin*/
	HRESULT BeginNonAnimInstance(shared_ptr<class CInstancingModel> _pModel, _uint _iMeshNum);

	/*터레인 같은 특수 버퍼(미리 생성 불가능한 것들) 전용 Begin*/
	HRESULT BeginShaderBuffer(shared_ptr<class CVIBuffer> _pVIBuffer);

public:
	HRESULT BindWVPMatrixPerspective(_float4x4 _worldMat);
	HRESULT BindWVPMatrixOrthographic(_float4x4 _worldMat);
	HRESULT BindRawValue(const _char* _pConstantName, const void* _pData, _uint _iLength);
	HRESULT BindMatrix(const _char* _pConstantName, const _float4x4* _pMatrix);
	HRESULT BindMatrices(const _char* _pConstantName, const _float4x4* _pMatrices, _uint _iNumMatrices);
	HRESULT BindSRV(const _char* _pConstantName, string _strTexTag);
	HRESULT BindSRVDirect(const _char* _pConstantName, wrl::ComPtr<ID3D11ShaderResourceView> _pSRV);
	HRESULT BindFloatVectorArray(const _char* _pConstantName, const _float4* _pVectors, _uint _iNumVectors);
	HRESULT BindUAV(const _char* _pConstantName, wrl::ComPtr<ID3D11UnorderedAccessView> _pUAV);

public:
	HRESULT BindHDRCube();
	HRESULT BindLightProjMatrix();
	HRESULT BindLightVPMatrix();

public:
	/*중간에 Pass Index를 바꿔야 할 때 사용*/
	void ChangePassIdx(_uint _iPassIdx) { m_iCurrentPassIdx = _iPassIdx; }

/*셰이더 로드*/
private:
	void ReadShaderFile(const wstring& _strBasePath);
	void ClassificationShaderFile(const wstring& _strFileName, const wstring& _strTotalPath);

/*공용으로 사용하는 버퍼 객체 Create*/
private:
	void CreateBuffers();

private:
	/*몇 번 텍스쳐가 있는지 확인한 후 Shader에 사용여부(bool)와 SRV를 던져준다*/
	/*ai Texture Type에 있는 텍스쳐를 전부 사용하지 않기 때문에 특정 번호를 지정해서 그 번호의 텍스쳐만 조회합니다.
	새로운 텍스쳐를 Shader에 추가했다면 bool 변수와 그 텍스쳐 번호 탐색을 추가하시면 됩니다*/
	void ModelMaterialAutoBind(shared_ptr<class CModel> _pModel, _uint _iMeshNum);
	void InstanceModelMaterialAutoBind(shared_ptr<class CInstancingModel> _pModel, _uint _iMeshNum);
	
/*ComputeShader용 SRV*/
private: 
	wrl::ComPtr<ID3D11UnorderedAccessView> m_pUAV = nullptr;
	wrl::ComPtr<ID3D11ShaderResourceView> m_pSRV = nullptr;
	wrl::ComPtr<ID3D11DepthStencilView> m_pDSV = nullptr;

public:
	/*PBR Ambient Lighting에 사용할 큐브를 생성*/
	HRESULT CreateCubeMap();


/*현재 Render에 사용하는 셰이더와 Pass Index*/
private:
	shared_ptr<class CShader> m_pCurrentShader = nullptr;
	ESHADER_TYPE m_eCurrentShaderType = ESHADER_TYPE::ST_END;
	_uint m_iCurrentPassIdx = 0;
	_uint m_iCurrentTechniqueIdx = 0;

/*Model Texture Bool Array*/
private:
	_bool m_bUsingTex[2] = { false, false };

/*Shader Files Path*/
private:
	const wstring& m_strClientShaderPath = TEXT("..\\..\\Client\\Bin\\ShaderFiles");
	const wstring& m_strEngineShaderPath = TEXT("..\\Bin\\ShaderFiles");

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	_float2 m_vScreenSize = _float2();

private:
	/*Shader List*/
	shared_ptr<class CShader> m_Shaders[ESHADER_TYPE::ST_END];
	shared_ptr<class CVIBuffer> m_Buffers[ESHADER_TYPE::ST_END];

	unordered_map<string, shared_ptr<class CVIBuffer>> m_BufferList;

private:
	wrl::ComPtr<ID3D11ShaderResourceView> m_pHDRCubeSRV = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> m_pHDRCubeRTV = nullptr;

public:
	static shared_ptr<CShaderHandler> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, _float _fScreenX, _float _fScreenY);

};

END
