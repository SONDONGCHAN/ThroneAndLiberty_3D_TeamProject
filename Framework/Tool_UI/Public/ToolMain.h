#pragma once

#include "Tool_Defines.h"
#include "Engine_Defines.h"

namespace Tool_UI
{
	class CToolMain final
	{
	public:
		CToolMain();
		~CToolMain();

	public:
		HRESULT Initialize();
		void Tick(_float _fTimeDelta);
		HRESULT Render();

	private:
		HRESULT OpenLevel(ETOOL_LEVEL _eStartLevel);

	public:
		void Free();

	};
}
