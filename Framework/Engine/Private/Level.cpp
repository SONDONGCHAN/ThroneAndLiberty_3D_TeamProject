#include "Level.h"
#include "GameInstance.h"

CLevel::CLevel()
{
}

HRESULT CLevel::Initialize()
{
    return S_OK;
}

void CLevel::Tick(_float _fTimeDelta)
{
}

void CLevel::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevel::Render()
{
    return S_OK;
}

HRESULT CLevel::ReadyLevelLight(wstring _strPath)
{
    wstring CompletePath = TEXT("..\\Bin\\DataFiles\\LightData\\");
    CompletePath = CompletePath + _strPath + TEXT(".json");

    Json::Value root =  GAMEINSTANCE->ReadJson(CompletePath);

    auto iter = root.begin();
    _int iCount = 0;

    LIGHT_DESC desc = {};
    ZeroMemory(&desc, sizeof(desc));

    for (iter; iter != root.end(); ++iter) {

        string strIndex = to_string(iCount);
        Json::Value CountValue = root[strIndex];


        _float4 vPosition = _float4(CountValue["Position"][0].asFloat(),
            CountValue["Position"][1].asFloat(),
            CountValue["Position"][2].asFloat(),
            1.f);

        _float4 vAmbient = _float4(CountValue["Ambient"][0].asFloat(),
            CountValue["Ambient"][1].asFloat(),
            CountValue["Ambient"][2].asFloat(),
            CountValue["Ambient"][3].asFloat());

        _float4 vDiffuse = _float4(CountValue["Diffuse"][0].asFloat(),
            CountValue["Diffuse"][1].asFloat(),
            CountValue["Diffuse"][2].asFloat(),
            CountValue["Diffuse"][3].asFloat());

        _float fRange = CountValue["Range"].asFloat();

        _bool UsingNoise = CountValue["UsingNoise"].asBool();

        _float fCullDist = CountValue["CullDistance"].asFloat();


        desc.eType = LIGHT_DESC::TYPE_POINT;
        desc.fRange = fRange;
        desc.vAmbient = vAmbient;
        desc.vDiffuse = vDiffuse;
        desc.vPosition = vPosition;
        desc.vSpecular = vDiffuse;
        desc.bUsingNoise = UsingNoise;
        desc.fCullDistance = fCullDist;

        if(FAILED(GAMEINSTANCE->AddLight(desc)))
            return E_FAIL;

        ++iCount;
    }


    return S_OK;
}

void CLevel::LevelBGMOn()
{
    for (auto& sound : m_AmbientSounds) {
        GAMEINSTANCE->PlayBGMFadeIn(sound.first, sound.second, 5.f);
        //GAMEINSTANCE->PlayBGM(sound.first, sound.second);
    }
}

void CLevel::LevelBGMOff()
{
    for (auto& sound : m_AmbientSounds) {
        GAMEINSTANCE->StopSoundFadeOut(sound.first,2.f);
        //GAMEINSTANCE->StopSound(sound.first);
    }
}



void CLevel::Free()
{
}
