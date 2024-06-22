#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CMaterial
{
public:
	CMaterial(unordered_map<_uint, string> _Textures);
	~CMaterial() = default;

public:
	unordered_map<_uint, string> GetTextures() { return m_Textures; }

private:
	unordered_map<_uint, string> m_Textures;

public:
	static shared_ptr<CMaterial> Create(unordered_map<_uint, string> _Textures);

};

END

