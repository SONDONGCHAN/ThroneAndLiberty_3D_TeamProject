#include "Material.h"

CMaterial::CMaterial(unordered_map<_uint, string> _Textures)
    :m_Textures(_Textures)

{
}

shared_ptr<CMaterial> CMaterial::Create(unordered_map<_uint, string> _Textures)
{
    shared_ptr<CMaterial> pInstance = make_shared<CMaterial>(_Textures);
    return pInstance;
}
