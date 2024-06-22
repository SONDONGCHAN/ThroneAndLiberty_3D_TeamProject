#pragma once

#include "VIInstancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIInstanceParticle final : public CVIInstancing
{
public:
	CVIInstanceParticle(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CVIInstanceParticle(const shared_ptr<CVIInstancing> _rhs);
	virtual ~CVIInstanceParticle();

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT InitializeClone(void* _pArg);

public:
	HRESULT			Update(PARTICLE* _pParticles, _int _iParticleCnt);

public:
	static shared_ptr<CVIInstanceParticle> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual shared_ptr<CComponent> Clone(void* _pArg);

};

END
