#pragma once

#include "Tool_CC_Define.h"

class CMainTool final
{
public:
	CMainTool();
	~CMainTool();

public:
	static shared_ptr<CMainTool> GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = make_shared<CMainTool>();
		}
		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (m_pInstance)
		{
			m_pInstance.reset();
		}
	}

public:
	HRESULT Initialize();
	void Tick(float _fTimeDelta);
	HRESULT Render();

private:
	HRESULT Setup_ImGui();

private:
	static shared_ptr<CMainTool> m_pInstance;
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

private:
	void Free();
};

