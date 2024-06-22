#include "EffectMgr.h"

#include "EffectGroup.h"
#include "EffectMesh.h"
#include "ParticleSystem.h"

IMPLEMENT_SINGLETON(Client::CEffectMgr)

CEffectMgr::CEffectMgr()
{
}

CEffectMgr::~CEffectMgr()
{
}

HRESULT CEffectMgr::Initialize()
{
	if (m_IsInitDone)
		return S_OK;

	// json 파일 다 읽어서 갖고있기
	LoadEffectFiles();

	// AnimEffect들 생성해주기
	AddAnimEffects();

	m_IsInitDone = true;

	return S_OK;
}

_int CEffectMgr::PlayEffect(string _strGroupID, shared_ptr<CGameObject> _pOwner, _bool _bForTest, _float _fTestStartFaster)
{
	auto it = m_EffectGroups.find(_strGroupID);

	// 해당 키값을 가진 이펙트 그룹이 없음
	if (it == m_EffectGroups.end())
	{
		return PlayAnimEffect(_strGroupID, _pOwner);
	}

 	vector<shared_ptr<CEffectGroup>> pEffectGroups = it->second;

	for (_int iIdx = 0; iIdx < pEffectGroups.size(); iIdx++)
	{
		if (!pEffectGroups[iIdx]->IsEnabled())
		{
 			pEffectGroups[iIdx]->Play(_pOwner, _bForTest, _fTestStartFaster);
			
			// 종료 용도로, 실행시킨 이펙트 그룹의 인덱스를 반환한다. (종료 시 인자로 넘겨주도록)
			return iIdx;
		}
	}

	// 실행 가능한 이펙트 그룹이 없을 경우
	return -1;
}

_int CEffectMgr::PlayEffect(string _strGroupID, _float3 _vWorldPos, _bool _bLoop, _float _fDuration)
{
	auto it = m_EffectGroups.find(_strGroupID);

	vector<shared_ptr<CEffectGroup>> pEffectGroups = it->second;

	for (_int iIdx = 0; iIdx < pEffectGroups.size(); iIdx++)
	{
		if (!pEffectGroups[iIdx]->IsEnabled())
		{
			pEffectGroups[iIdx]->Play(_vWorldPos, _bLoop, _fDuration);

			// 종료 용도로, 실행시킨 이펙트 그룹의 인덱스를 반환한다. (종료 시 인자로 넘겨주도록)
			return iIdx;
		}
	}

	// 실행 가능한 이펙트 그룹이 없을 경우
	return -1;
}

_int CEffectMgr::PlayAnimEffect(string _strGroupID, shared_ptr<CGameObject> _pOwner)
{
	auto it = m_AnimEffects.find(_strGroupID);

	// 해당 키값을 가진 이펙트 그룹이 없음
	if (it == m_AnimEffects.end())
	{
		return -1;
	}

	vector<shared_ptr<CAnimEffect>> pAnimEffect = it->second;

	for (_int iIdx = 0; iIdx < pAnimEffect.size(); iIdx++)
	{
		if (!pAnimEffect[iIdx]->IsEnabled())
		{
			pAnimEffect[iIdx]->Play(_pOwner);

			// 종료 용도로, 실행시킨 이펙트 그룹의 인덱스를 반환한다. (종료 시 인자로 넘겨주도록)
			return iIdx;
		}
	}
}

HRESULT CEffectMgr::StopEffect(string _strGroupID, _int _iIdx)
{
	auto it = m_EffectGroups.find(_strGroupID);

	// 해당 키값을 가진 이펙트 그룹이 없음
	if (it == m_EffectGroups.end())
		return E_FAIL;

	// 해당 키값을 가진 이펙트 그룹들의 벡터 인덱스를 넘어감
	if (it->second.size() <= _iIdx)
		return E_FAIL;

	it->second[_iIdx]->StopEffect();

	return S_OK;
}

HRESULT CEffectMgr::AddEffect(string _strGroupID, _int _iCnt)
{
	return S_OK;
}

HRESULT CEffectMgr::RemoveEffect(string _strGroupID, _int _iCnt)
{
	return S_OK;
}

HRESULT CEffectMgr::LoadEffectFiles()
{
	m_EffectGroups.clear();

	filesystem::path basePath(m_strFilePath);

	for (const auto& entry : filesystem::directory_iterator(basePath)) {
		if (entry.is_regular_file()) {
			if (".json" == entry.path().extension()) {
				if (FAILED(LoadEffectGroup(entry.path().filename().stem().string())))
					return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CEffectMgr::LoadEffectGroup(string _strGroupKey)
{
	string strFilePath = m_strFilePath + _strGroupKey + ".json";

	Json::Value root;

	_tchar tFinalPath[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, strFilePath.c_str(), static_cast<_uint>(strlen(strFilePath.c_str())), tFinalPath, MAX_PATH);

	wstring wstrFinalPath = tFinalPath;

	root = GAMEINSTANCE->ReadJson(wstrFinalPath);

	Json::Value GroupInfo = root[_strGroupKey];

	_int _iEffectCnt = GroupInfo["EffectCnt"].asInt();

	EFFECT_ATTACH_TYPE eEffectAttachType = (EFFECT_ATTACH_TYPE)GroupInfo["EffectAttachType"].asInt();

	Json::Value Element = GroupInfo["Effects"];

	_int iCloneCnt = GroupInfo["CloneCnt"].asInt();

	vector<shared_ptr<CEffectGroup>> EffectGroupVec;

	for (_int i = 0; i < iCloneCnt; ++i)
	{
		Json::ValueIterator iter = Element.begin();

		shared_ptr<CEffectGroup> pEffectGroup = CEffectGroup::Create(eEffectAttachType);

		for (_int j = 0; j < _iEffectCnt; ++j)
		{
			string	strEffectID = iter.key().asString();

			if (FAILED(LoadEffects(Element, strEffectID, pEffectGroup)))
				return E_FAIL;
			iter++;
		}

		EffectGroupVec.push_back(pEffectGroup);

		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), pEffectGroup)))
			return E_FAIL;
	}/*

	shared_ptr<CEffectGroup> pClonedEffectGroup;

	for (_int i = 0; i < iCloneCnt; ++i)
	{
		pClonedEffectGroup = make_shared<CEffectGroup>(*pEffectGroup);
		EffectGroupVec.push_back(pClonedEffectGroup);

		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_Effect"), pClonedEffectGroup)))
			return E_FAIL;
	}*/

	m_EffectGroups.emplace(_strGroupKey, EffectGroupVec);

	return S_OK;
}

HRESULT CEffectMgr::LoadEffects(Json::Value& _Element, string _strEffectID, shared_ptr<CEffectGroup> pEffectGroup)
{
	Json::Value& Element = _Element[_strEffectID];

	_int	iEffectType = Element["Type"].asInt();

	Json::Value& Desc = Element["Desc"];

	if (CEffect::EFFECT_MESH == iEffectType)
	{
		// 메쉬
		CEffectMesh::EffectMeshDesc eEffectMeshDesc = {};

		eEffectMeshDesc.iEffectType = iEffectType;
		eEffectMeshDesc.iShaderType = Desc["ShaderType"].asInt();
		eEffectMeshDesc.iShaderPass = Desc["ShaderPass"].asInt();
		eEffectMeshDesc.iRenderGroup = Desc["RenderGroup"].asInt();

		eEffectMeshDesc.fDuration = Desc["Duration"].asFloat();
		eEffectMeshDesc.StartTrackPos = Desc["StartTrackPos"].asDouble();

		eEffectMeshDesc.bLoop = Desc["Loop"].asBool();
		eEffectMeshDesc.bUseDiffuse = Desc["UseDiffuse"].asBool();
		eEffectMeshDesc.strTexDiffuseKey = Desc["TexDiffuseKey"].asString();

		eEffectMeshDesc.strModelKey = Desc["ModelKey"].asString();
		eEffectMeshDesc.bBillBoard = Desc["BillBoard"].asBool();
		eEffectMeshDesc.bUseMask = Desc["UseMask"].asBool();
		eEffectMeshDesc.strTexMaskKey = Desc["TexMaskKey"].asString();
		eEffectMeshDesc.bUseNoise = Desc["UseNoise"].asBool();
		eEffectMeshDesc.strTexNoiseKey = Desc["TexNoiseKey"].asString();
		eEffectMeshDesc.bUseColor = Desc["UseColor"].asBool();

		eEffectMeshDesc.bUseDistortion = Desc["UseDistortion"].asBool();
		eEffectMeshDesc.strDistortionKey = Desc["DistortionKey"].asString();
		eEffectMeshDesc.fDistortionSpeed = Desc["DistortionSpeed"].asFloat();
		eEffectMeshDesc.fDistortionWeight = Desc["DistortionWeight"].asFloat();

		if (Desc.isMember("UseDslv"))
		{
			eEffectMeshDesc.bUseDslv = Desc["UseDslv"].asBool();
			eEffectMeshDesc.iDslvType = Desc["DslvType"].asInt();
			eEffectMeshDesc.bDslvGlow = Desc["DslvGlow"].asBool();
			eEffectMeshDesc.fDslvStartTime = Desc["DslvStartTime"].asFloat();
			eEffectMeshDesc.fDslvDuration = Desc["DslvDuration"].asFloat();
			eEffectMeshDesc.fDslvThick = Desc["DslvThick"].asFloat();
			eEffectMeshDesc.strDslvTexKey = Desc["strDslvTexKey"].asString();

			if (Desc["DslvColor"].isArray())
			{
				Json::Value DslvColor = Desc["DslvColor"];
				eEffectMeshDesc.vDslvColor = _float4(DslvColor[0].asFloat(), DslvColor[1].asFloat(), DslvColor[2].asFloat(), DslvColor[3].asFloat());
			}

			if (Desc["DslvGlowColor"].isArray())
			{
				Json::Value DslvGlowColor = Desc["DslvGlowColor"];
				eEffectMeshDesc.vDslvGlowColor = _float4(DslvGlowColor[0].asFloat(), DslvGlowColor[1].asFloat(), DslvGlowColor[2].asFloat(), DslvGlowColor[3].asFloat());
			}
		}

		else
		{
			eEffectMeshDesc.bUseDslv = false;
			eEffectMeshDesc.iDslvType = 0;
			eEffectMeshDesc.bDslvGlow = false;
			eEffectMeshDesc.fDslvStartTime = 0.f;
			eEffectMeshDesc.fDslvDuration = 0.f;
			eEffectMeshDesc.fDslvThick = 0.f;
			eEffectMeshDesc.strDslvTexKey = "";

			eEffectMeshDesc.vDslvColor = { 1.f, 1.f, 1.f, 1.f };
			eEffectMeshDesc.vDslvGlowColor = { 1.f, 1.f, 1.f, 1.f };
		}

		eEffectMeshDesc.iDiffuseSamplerType = Desc["DiffuseSamplerType"].asInt();
		eEffectMeshDesc.iDiffuseUVType = Desc["DiffuseUVType"].asInt();
		eEffectMeshDesc.fDiffuseDiscardValue = Desc["DiffuseDiscardValue"].asFloat();
		eEffectMeshDesc.iDiffuseDiscardArea = Desc["DiffuseDiscardArea"].asInt();

		if (Desc["DiffuseUVScroll"].isArray())
		{
			Json::Value DiffuseUVScroll = Desc["DiffuseUVScroll"];
			eEffectMeshDesc.fDiffuseUVScroll = _float2(DiffuseUVScroll[0].asFloat(), DiffuseUVScroll[1].asFloat());
		}

		if (Desc["DiffuseUVAtlasCnt"].isArray())
		{
			Json::Value DiffuseUVAtlasCnt = Desc["DiffuseUVAtlasCnt"];
			eEffectMeshDesc.iDiffuseUVAtlasCnt = _float2(DiffuseUVAtlasCnt[0].asFloat(), DiffuseUVAtlasCnt[1].asFloat());
		}

		eEffectMeshDesc.fDiffuseUVAtlasSpeed = Desc["DiffuseUVAtlasSpeed"].asFloat();

		eEffectMeshDesc.iMaskSamplerType = Desc["MaskSamplerType"].asInt();
		eEffectMeshDesc.iMaskUVType = Desc["MaskUVType"].asInt();
		eEffectMeshDesc.fMaskDiscardValue = Desc["MaskDiscardValue"].asFloat();
		eEffectMeshDesc.iMaskDiscardArea = Desc["MaskDiscardArea"].asInt();

		if (Desc.isMember("MaskUVScroll") && Desc["MaskUVScroll"].isArray() && Desc["MaskUVScroll"].size() == 2) {
			Json::Value maskUVScroll = Desc["MaskUVScroll"];
			eEffectMeshDesc.fMaskUVScroll = _float2(maskUVScroll[0].asFloat(), maskUVScroll[1].asFloat());
		}

		if (Desc["MaskUVAtlasCnt"].isArray())
		{
			Json::Value MaskUVAtlasCnt = Desc["MaskUVAtlasCnt"];
			eEffectMeshDesc.iMaskUVAtlasCnt = _float2(MaskUVAtlasCnt[0].asFloat(), MaskUVAtlasCnt[1].asFloat());
		}

		eEffectMeshDesc.fMaskUVAtlasSpeed = Desc["MaskUVAtlasSpeed"].asFloat();

		eEffectMeshDesc.iNoiseSamplerType = Desc["NoiseSamplerType"].asInt();
		eEffectMeshDesc.iNoiseUVType = Desc["NoiseUVType"].asInt();
		eEffectMeshDesc.fNoiseDiscardValue = Desc["NoiseDiscardValue"].asFloat();
		eEffectMeshDesc.iNoiseDiscardArea = Desc["NoiseDiscardArea"].asInt();

		if (Desc["NoiseUVScroll"].isArray())
		{
			Json::Value NoiseUVScroll = Desc["NoiseUVScroll"];
			eEffectMeshDesc.fNoiseUVScroll = _float2(NoiseUVScroll[0].asFloat(), NoiseUVScroll[1].asFloat());
		}

		if (Desc["NoiseUVAtlasCnt"].isArray())
		{
			Json::Value NoiseUVAtlasCnt = Desc["NoiseUVAtlasCnt"];
			eEffectMeshDesc.iNoiseUVAtlasCnt = _float2(NoiseUVAtlasCnt[0].asFloat(), NoiseUVAtlasCnt[1].asFloat());
		}

		eEffectMeshDesc.fNoiseUVAtlasSpeed = Desc["NoiseUVAtlasSpeed"].asFloat();

		eEffectMeshDesc.iColorSetCnt = Desc["ColorSetCnt"].asInt();
		eEffectMeshDesc.iColorSplitArea = Desc["ColorSplitArea"].asInt();

		if (Desc["ColorSplitter"].isArray())
		{
			Json::Value ColorSplitter = Desc["ColorSplitter"];
			eEffectMeshDesc.vColorSplitter = _float4(ColorSplitter[0].asFloat(), ColorSplitter[1].asFloat(), ColorSplitter[2].asFloat(), ColorSplitter[3].asFloat());
		}

		eEffectMeshDesc.iScaleFollowType = Desc["ScaleFollowType"].asInt();
		eEffectMeshDesc.strScaleFollowBoneKey = Desc["ScaleFollowBoneKey"].asString();
		eEffectMeshDesc.iScaleSetCnt = Desc["ScaleSetCnt"].asInt();
		eEffectMeshDesc.iRotFollowType = Desc["RotFollowType"].asInt();
		eEffectMeshDesc.strRotFollowBoneKey = Desc["RotFollowBoneKey"].asString();

		if (Desc["Rotation"].isArray())
		{
			Json::Value Rotation = Desc["Rotation"];
			eEffectMeshDesc.vRotation = _float3(Rotation[0].asFloat(), Rotation[1].asFloat(), Rotation[2].asFloat());
		}

		if (Desc["TurnVel"].isArray())
		{
			Json::Value TurnVel = Desc["TurnVel"];
			eEffectMeshDesc.vTurnVel = _float3(TurnVel[0].asFloat(), TurnVel[1].asFloat(), TurnVel[2].asFloat());
		}

		if (Desc["TurnAcc"].isArray())
		{
			Json::Value TurnAcc = Desc["TurnAcc"];
			eEffectMeshDesc.vTurnAcc = _float3(TurnAcc[0].asFloat(), TurnAcc[1].asFloat(), TurnAcc[2].asFloat());
		}

		eEffectMeshDesc.iTransFollowType = Desc["TransFollowType"].asInt();
		eEffectMeshDesc.strTransFollowBoneKey = Desc["TransFollowBoneKey"].asString();
		eEffectMeshDesc.iTransSetCnt = Desc["TransSetCnt"].asInt();

		eEffectMeshDesc.iGlowColorSetCnt = Desc["GlowColorSetCnt"].asInt();
		eEffectMeshDesc.bUseGlowColor = Desc["UseGlowColor"].asBool();

		Json::Value& KeyFrames = Desc["KeyFrames"];

		Json::Value& ScaleKeyFrame = KeyFrames["Scale"];
		Json::Value& TransKeyFrame = KeyFrames["Trans"];
		Json::Value& ColorKeyFrame = KeyFrames["Color"];
		Json::Value& GlowColorKeyFrame = KeyFrames["GlowColor"];

		{
			Json::Value& ScaleKeyFrameTime = ScaleKeyFrame["Time"];
			Json::Value& ScaleKeyFrameEase = ScaleKeyFrame["EaseType"];
			Json::Value& ScaleKeyFrameScale = ScaleKeyFrame["Scale"];

			_float	fScaleTime[MAX_TIMETABLE] = {};
			_int	iScaleEase[MAX_TIMETABLE] = {};
			_float3	fScale[MAX_TIMETABLE] = {};

			if (ScaleKeyFrameTime.isArray())
			{
				_int iScaleIdx = 0;

				for (const auto& ScaleTime : ScaleKeyFrameTime)
				{
					fScaleTime[iScaleIdx++] = ScaleTime.asFloat();
				}

				iScaleIdx = 0;

				for (const auto& ScaleEase : ScaleKeyFrameEase)
				{
					iScaleEase[iScaleIdx++] = ScaleEase.asInt();
				}

				iScaleIdx = 0;

				for (const auto& ScaleValue : ScaleKeyFrameScale)
				{
					fScale[iScaleIdx++] = _float3(ScaleValue[0].asFloat(), ScaleValue[1].asFloat(), ScaleValue[2].asFloat());
				}
			}

			CEffectMesh::SCALE_KEYFRAME scaleKeyFrame;

			for (_int i = 0; i < eEffectMeshDesc.iScaleSetCnt; ++i)
			{
				scaleKeyFrame.fTime = fScaleTime[i];
				scaleKeyFrame.eEaseType = iScaleEase[i];
				scaleKeyFrame.vScale = fScale[i];

				eEffectMeshDesc.KeyFrames.ScaleKeyFrames.push_back(scaleKeyFrame);
			}
		}

		{
			Json::Value& TransKeyFrameTime = TransKeyFrame["Time"];
			Json::Value& TransKeyFrameEase = TransKeyFrame["EaseType"];
			Json::Value& TransKeyFrameTrans = TransKeyFrame["Trans"];

			_float	fTransTime[MAX_TIMETABLE] = {};
			_int	iTransEase[MAX_TIMETABLE] = {};
			_float3	fTrans[MAX_TIMETABLE] = {};

			if (TransKeyFrameTime.isArray())
			{
				_int iTransIdx = 0;

				for (const auto& TransTime : TransKeyFrameTime)
				{
					fTransTime[iTransIdx++] = TransTime.asFloat();
				}

				iTransIdx = 0;

				for (const auto& TransEase : TransKeyFrameEase)
				{
					iTransEase[iTransIdx++] = TransEase.asInt();
				}

				iTransIdx = 0;

				for (const auto& TransValue : TransKeyFrameTrans)
				{
					fTrans[iTransIdx++] = _float3(TransValue[0].asFloat(), TransValue[1].asFloat(), TransValue[2].asFloat());
				}
			}

			CEffectMesh::TRANS_KEYFRAME transKeyFrame;

			for (_int i = 0; i < eEffectMeshDesc.iTransSetCnt; ++i)
			{
				transKeyFrame.fTime = fTransTime[i];
				transKeyFrame.eEaseType = iTransEase[i];
				transKeyFrame.vTranslation = fTrans[i];

				eEffectMeshDesc.KeyFrames.TransKeyFrames.push_back(transKeyFrame);
			}
		}

		{
			Json::Value& GlowColorKeyFrameTime = GlowColorKeyFrame["Time"];
			Json::Value& GlowColorKeyFrameColor = GlowColorKeyFrame["GlowColor"];

			_float	fTime[MAX_TIMETABLE] = {};
			_float4	fGlowColor[MAX_TIMETABLE] = {};

			if (GlowColorKeyFrameTime.isArray())
			{
				_int iTransIdx = 0;

				for (const auto& GlowTime : GlowColorKeyFrameTime)
				{
					fTime[iTransIdx++] = GlowTime.asFloat();
				}
				iTransIdx = 0;

				for (const auto& ColorValue : GlowColorKeyFrameColor)
				{
					fGlowColor[iTransIdx++] = _float4(ColorValue[0].asFloat(), ColorValue[1].asFloat(), ColorValue[2].asFloat(), ColorValue[3].asFloat());
				}
			}

			CEffectMesh::GLOWCOLOR_KEYFRAME glowColorKeyFrame;

			for (_int i = 0; i < eEffectMeshDesc.iGlowColorSetCnt; ++i)
			{
				glowColorKeyFrame.fTime = fTime[i];
				glowColorKeyFrame.vGlowColor = fGlowColor[i];

				eEffectMeshDesc.KeyFrames.GlowColorKeyFrames.push_back(glowColorKeyFrame);
			}
		}

		{
			Json::Value& ColorKeyFrameTime = ColorKeyFrame["Time"];
			Json::Value& ColorKeyFrameEase = ColorKeyFrame["EaseType"];
			Json::Value& ColorKeyFrameColor = ColorKeyFrame["Color"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float4	vColor[MAX_TIMETABLE][4] = {};

			if (ColorKeyFrameTime.isArray())
			{
				_int iIdx = 0;

				for (const auto& ColorTime : ColorKeyFrameTime)
				{
					fTime[iIdx++] = ColorTime.asFloat();
				}

				iIdx = 0;

				for (const auto& ColorEase : ColorKeyFrameEase)
				{
					iEase[iIdx++] = ColorEase.asInt();
				}

				iIdx = 0;

				for (const auto& ColorArray : ColorKeyFrameColor)
				{
					_int iSplit = 0;

					for (const auto& Color : ColorArray)
					{
						vColor[iIdx][iSplit++] = _float4(Color[0].asFloat(), Color[1].asFloat(), Color[2].asFloat(), Color[3].asFloat());
					}
					iIdx++;
				}
			}

			CEffectMesh::COLOR_KEYFRAME colorKeyFrame;

			for (_int i = 0; i < eEffectMeshDesc.iColorSetCnt; ++i)
			{
				colorKeyFrame.fTime = fTime[i];
				colorKeyFrame.eEaseType = iEase[i];
				for (_int j = 0; j < eEffectMeshDesc.iColorSplitArea; j++)
					colorKeyFrame.vColor[j] = vColor[i][j];

				eEffectMeshDesc.KeyFrames.ColorKeyFrames.push_back(colorKeyFrame);
			}
		}

		shared_ptr<CEffectMesh> pEffectMesh = CEffectMesh::Create(&eEffectMeshDesc);

		// 여기
		pEffectGroup->AddEffect(_strEffectID, pEffectMesh);
	}

	else if (CEffect::EFFECT_PARTICLE == iEffectType)
	{
		// 파티클
		CParticleSystem::PARTICLE_SYSTEM eParticleDesc = {};

		eParticleDesc.iEffectType = iEffectType;
		eParticleDesc.iShaderType = Desc["ShaderType"].asInt();
		eParticleDesc.iShaderPass = Desc["ShaderPass"].asInt();
		eParticleDesc.iRenderGroup = Desc["RenderGroup"].asInt();

		eParticleDesc.fDuration = Desc["Duration"].asFloat();
		eParticleDesc.StartTrackPos = Desc["StartTrackPos"].asDouble();

		eParticleDesc.bLoop = Desc["Loop"].asBool();
		eParticleDesc.bUseDiffuse = Desc["UseDiffuse"].asBool();
		eParticleDesc.strTexDiffuseKey = Desc["TexDiffuseKey"].asString();

		eParticleDesc.bUseHazeEffect = Desc["UseHazeEffect"].asBool();

		if (Desc["HazeSpeedMin"].isArray())
		{
			Json::Value HazeSpeedMin = Desc["HazeSpeedMin"];
			eParticleDesc.vHazeSpeedMin = _float3(HazeSpeedMin[0].asFloat(), HazeSpeedMin[1].asFloat(), HazeSpeedMin[2].asFloat());
		}

		if (Desc["HazeSpeedMax"].isArray())
		{
			Json::Value HazeSpeedMax = Desc["HazeSpeedMax"];
			eParticleDesc.vHazeSpeedMax = _float3(HazeSpeedMax[0].asFloat(), HazeSpeedMax[1].asFloat(), HazeSpeedMax[2].asFloat());
		}

		eParticleDesc.iMaxParticles = Desc["MaxParticles"].asInt();
		eParticleDesc.iSpawnType = Desc["SpawnType"].asInt();

		if (Desc.isMember("LifeTime") && Desc["LifeTime"].isArray() && Desc["LifeTime"].size() == 2) {
			Json::Value LifeTime = Desc["LifeTime"];
			eParticleDesc.vLifeTime = _float2(LifeTime[0].asFloat(), LifeTime[1].asFloat());
		}

		eParticleDesc.fSpawnNumPerSec = Desc["SpawnNumPerSec"].asFloat();
		eParticleDesc.iRenderType = Desc["RenderType"].asInt();
		eParticleDesc.iBillBoardType = Desc["BillBoardType"].asInt();

		eParticleDesc.iPosType = Desc["PosType"].asInt();
		/*if (eParticleDesc.iMaxParticles == 301)
			int a = 3;*/
		eParticleDesc.bPosFollow = Desc["PosFollow"].asBool();
		eParticleDesc.bUsePosKeyFrame = false;
		if (Desc.isMember("UsePosKeyFrame"))
		{
			eParticleDesc.bUsePosKeyFrame = Desc["UsePosKeyFrame"].asBool();
		}
		eParticleDesc.strPosFollowBoneKey = Desc["PosFollowBoneKey"].asString();

		if (Desc.isMember("RelativeOwner"))
			eParticleDesc.bRelativeOwner = Desc["RelativeOwner"].asBool();
		else
			eParticleDesc.bRelativeOwner = false;

		eParticleDesc.bUseGlowColor = Desc["UseGlowColor"].asBool();

		if (Desc["GlowColor"].isArray())
		{
			Json::Value GlowColor = Desc["GlowColor"];
			eParticleDesc.vGlowColor = _float4(GlowColor[0].asFloat(), GlowColor[1].asFloat(), GlowColor[2].asFloat(), GlowColor[3].asFloat());
		}

		if (!eParticleDesc.bUsePosKeyFrame)
		{
			if (Desc["PosMin"].isArray())
			{
				Json::Value PosMin = Desc["PosMin"];
				eParticleDesc.vPosMin = _float3(PosMin[0].asFloat(), PosMin[1].asFloat(), PosMin[2].asFloat());
			}

			if (Desc["PosMax"].isArray())
			{
				Json::Value PosMax = Desc["PosMax"];
				eParticleDesc.vPosMax = _float3(PosMax[0].asFloat(), PosMax[1].asFloat(), PosMax[2].asFloat());
			}
		}

		eParticleDesc.bUsePivot = Desc["UsePivot"].asBool();

		if (Desc["PivotMin"].isArray())
		{
			Json::Value PivotMin = Desc["PivotMin"];
			eParticleDesc.vPivotMin = _float3(PivotMin[0].asFloat(), PivotMin[1].asFloat(), PivotMin[2].asFloat());
		}

		if (Desc["PivotMax"].isArray())
		{
			Json::Value PivotMax = Desc["PivotMax"];
			eParticleDesc.vPivotMax = _float3(PivotMax[0].asFloat(), PivotMax[1].asFloat(), PivotMax[2].asFloat());
		}

		eParticleDesc.iShape = Desc["Shape"].asInt();
		eParticleDesc.bSpeedMode = Desc["SpeedMode"].asBool();

		if (Desc["StartSpeedMin"].isArray())
		{
			Json::Value StartSpeedMin = Desc["StartSpeedMin"];
			eParticleDesc.vStartSpeedMin = _float3(StartSpeedMin[0].asFloat(), StartSpeedMin[1].asFloat(), StartSpeedMin[2].asFloat());
		}

		if (Desc["StartSpeedMax"].isArray())
		{
			Json::Value StartSpeedMax = Desc["StartSpeedMax"];
			eParticleDesc.vStartSpeedMax = _float3(StartSpeedMax[0].asFloat(), StartSpeedMax[1].asFloat(), StartSpeedMax[2].asFloat());
		}

		if (Desc["SpeedAccMin"].isArray())
		{
			Json::Value SpeedAccMin = Desc["SpeedAccMin"];
			eParticleDesc.vSpeedAccMin = _float3(SpeedAccMin[0].asFloat(), SpeedAccMin[1].asFloat(), SpeedAccMin[2].asFloat());
		}

		if (Desc["SpeedAccMax"].isArray())
		{
			Json::Value SpeedAccMax = Desc["SpeedAccMax"];
			eParticleDesc.vSpeedAccMax = _float3(SpeedAccMax[0].asFloat(), SpeedAccMax[1].asFloat(), SpeedAccMax[2].asFloat());
		}


		if (Desc["RotationMin"].isArray())
		{
			Json::Value RotationMin = Desc["RotationMin"];
			eParticleDesc.vRotationMin = _float3(RotationMin[0].asFloat(), RotationMin[1].asFloat(), RotationMin[2].asFloat());
		}

		if (Desc["RotationMax"].isArray())
		{
			Json::Value RotationMax = Desc["RotationMax"];
			eParticleDesc.vRotationMax = _float3(RotationMax[0].asFloat(), RotationMax[1].asFloat(), RotationMax[2].asFloat());
		}

		if (Desc["TurnVelMin"].isArray())
		{
			Json::Value TurnVelMin = Desc["TurnVelMin"];
			eParticleDesc.vTurnVelMin = _float3(TurnVelMin[0].asFloat(), TurnVelMin[1].asFloat(), TurnVelMin[2].asFloat());
		}

		if (Desc["TurnVelMax"].isArray())
		{
			Json::Value TurnVelMax = Desc["TurnVelMax"];
			eParticleDesc.vTurnVelMax = _float3(TurnVelMax[0].asFloat(), TurnVelMax[1].asFloat(), TurnVelMax[2].asFloat());
		}

		if (Desc["TurnAccMin"].isArray())
		{
			Json::Value TurnAccMin = Desc["TurnAccMin"];
			eParticleDesc.vTurnAccMin = _float3(TurnAccMin[0].asFloat(), TurnAccMin[1].asFloat(), TurnAccMin[2].asFloat());
		}

		if (Desc["TurnAccMax"].isArray())
		{
			Json::Value TurnAccMax = Desc["TurnAccMax"];
			eParticleDesc.vTurnAccMax = _float3(TurnAccMax[0].asFloat(), TurnAccMax[1].asFloat(), TurnAccMax[2].asFloat());
		}

		eParticleDesc.bUseColor = Desc["UseColor"].asBool();

		Json::Value& KeyFrames = Desc["KeyFrames"];

		Json::Value& ScaleKeyFrame = KeyFrames["Scale"];
		Json::Value& SpeedKeyFrame = KeyFrames["Speed"];
		Json::Value& ColorKeyFrame = KeyFrames["Color"];
		Json::Value& PosKeyFrame = KeyFrames["Pos"];

		{
			Json::Value& ScaleKeyFrameTime = ScaleKeyFrame["Time"];
			Json::Value& ScaleKeyFrameEase = ScaleKeyFrame["EaseType"];
			Json::Value& ScaleKeyFrameScaleMin = ScaleKeyFrame["ScaleMin"];
			Json::Value& ScaleKeyFrameScaleMax = ScaleKeyFrame["ScaleMax"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float3	vMin[MAX_TIMETABLE] = {};
			_float3	vMax[MAX_TIMETABLE] = {};

			if (ScaleKeyFrameTime.isArray())
			{
				_int iScaleIdx = 0;

				for (const auto& ScaleTime : ScaleKeyFrameTime)
				{
					fTime[iScaleIdx++] = ScaleTime.asFloat();
				}

				iScaleIdx = 0;

				for (const auto& ScaleEase : ScaleKeyFrameEase)
				{
					iEase[iScaleIdx++] = ScaleEase.asInt();
				}

				iScaleIdx = 0;

				for (const auto& ScaleValue : ScaleKeyFrameScaleMin)
				{
					vMin[iScaleIdx++] = _float3(ScaleValue[0].asFloat(), ScaleValue[1].asFloat(), ScaleValue[2].asFloat());
				}

				iScaleIdx = 0;

				for (const auto& ScaleValue : ScaleKeyFrameScaleMax)
				{
					vMax[iScaleIdx++] = _float3(ScaleValue[0].asFloat(), ScaleValue[1].asFloat(), ScaleValue[2].asFloat());
				}
			}

			CParticleSystem::SCALE_KEYFRAME_PARTICLE scaleKeyFrame;

			for (_int i = 0; i < Desc["ScaleSetCnt"].asInt(); ++i)
			{
				scaleKeyFrame.fTime = fTime[i];
				scaleKeyFrame.eEaseType = iEase[i];
				scaleKeyFrame.vScaleMin = vMin[i];
				scaleKeyFrame.vScaleMax = vMax[i];

				eParticleDesc.KeyFrames.ScaleKeyFrames.push_back(scaleKeyFrame);
			}
		}

		if (eParticleDesc.bUsePosKeyFrame)
		{
			Json::Value& PosKeyFrameTime = PosKeyFrame["Time"];
			Json::Value& PosKeyFrameEase = PosKeyFrame["EaseType"];
			Json::Value& PosKeyFramePosMin = PosKeyFrame["PosMin"];
			Json::Value& PosKeyFramePosMax = PosKeyFrame["PosMax"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float3	vMin[MAX_TIMETABLE] = {};
			_float3	vMax[MAX_TIMETABLE] = {};

			if (PosKeyFrameTime.isArray())
			{
				_int iPosIdx = 0;

				for (const auto& PosTime : PosKeyFrameTime)
				{
					fTime[iPosIdx++] = PosTime.asFloat();
				}

				iPosIdx = 0;

				for (const auto& PosEase : PosKeyFrameEase)
				{
					iEase[iPosIdx++] = PosEase.asInt();
				}

				iPosIdx = 0;

				for (const auto& PosValue : PosKeyFramePosMin)
				{
					vMin[iPosIdx++] = _float3(PosValue[0].asFloat(), PosValue[1].asFloat(), PosValue[2].asFloat());
				}

				iPosIdx = 0;

				for (const auto& PosValue : PosKeyFramePosMax)
				{
					vMax[iPosIdx++] = _float3(PosValue[0].asFloat(), PosValue[1].asFloat(), PosValue[2].asFloat());
				}
			}

			CParticleSystem::POSITION_KEYFRAME_PARTICLE PosKeyFrame;

			for (_int i = 0; i < Desc["PosSetCnt"].asInt(); ++i)
			{
				PosKeyFrame.fTime = fTime[i];
				PosKeyFrame.eEaseType = iEase[i];
				PosKeyFrame.vPosMin = vMin[i];
				PosKeyFrame.vPosMax = vMax[i];

				eParticleDesc.KeyFrames.PosKeyFrames.push_back(PosKeyFrame);
			}
		}

		{
			Json::Value& SpeedKeyFrameTime = SpeedKeyFrame["Time"];
			Json::Value& SpeedKeyFrameEase = SpeedKeyFrame["EaseType"];
			Json::Value& SpeedKeyFrameMin = SpeedKeyFrame["SpeedMin"];
			Json::Value& SpeedKeyFrameMax = SpeedKeyFrame["SpeedMax"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float3	vMin[MAX_TIMETABLE] = {};
			_float3	vMax[MAX_TIMETABLE] = {};

			if (SpeedKeyFrameTime.isArray())
			{
				_int iIdx = 0;

				for (const auto& Time : SpeedKeyFrameTime)
				{
					fTime[iIdx++] = Time.asFloat();
				}

				iIdx = 0;

				for (const auto& Ease : SpeedKeyFrameEase)
				{
					iEase[iIdx++] = Ease.asInt();
				}

				iIdx = 0;

				for (const auto& Min : SpeedKeyFrameMin)
				{
					vMin[iIdx++] = _float3(Min[0].asFloat(), Min[1].asFloat(), Min[2].asFloat());
				}

				iIdx = 0;

				for (const auto& Max : SpeedKeyFrameMax)
				{
					vMax[iIdx++] = _float3(Max[0].asFloat(), Max[1].asFloat(), Max[2].asFloat());
				}
			}

			CParticleSystem::SPEED_KEYFRAME_PARTICLE speedKeyFrame;

			for (_int i = 0; i < Desc["SpeedSetCnt"].asInt(); ++i)
			{
				speedKeyFrame.fTime = fTime[i];
				speedKeyFrame.eEaseType = iEase[i];
				speedKeyFrame.vSpeedMin = vMin[i];
				speedKeyFrame.vSpeedMax = vMax[i];

				eParticleDesc.KeyFrames.SpeedKeyFrames.push_back(speedKeyFrame);
			}
		}

		{
			Json::Value& ColorKeyFrameTime = ColorKeyFrame["Time"];
			Json::Value& ColorKeyFrameEase = ColorKeyFrame["EaseType"];
			Json::Value& ColorKeyFrameMin = ColorKeyFrame["ColorMin"];
			Json::Value& ColorKeyFrameMax = ColorKeyFrame["ColorMax"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float4	vMin[MAX_TIMETABLE] = {};
			_float4	vMax[MAX_TIMETABLE] = {};

			if (ColorKeyFrameTime.isArray())
			{
				_int iIdx = 0;

				for (const auto& Time : ColorKeyFrameTime)
				{
					fTime[iIdx++] = Time.asFloat();
				}

				iIdx = 0;

				for (const auto& Ease : ColorKeyFrameEase)
				{
					iEase[iIdx++] = Ease.asInt();
				}

				iIdx = 0;

				for (const auto& Min : ColorKeyFrameMin)
				{
					vMin[iIdx++] = _float4(Min[0].asFloat(), Min[1].asFloat(), Min[2].asFloat(), Min[3].asFloat());
				}

				iIdx = 0;

				for (const auto& Max : ColorKeyFrameMax)
				{
					vMax[iIdx++] = _float4(Max[0].asFloat(), Max[1].asFloat(), Max[2].asFloat(), Max[3].asFloat());
				}
			}

			CParticleSystem::COLOR_KEYFRAME_PARTICLE colorKeyFrame;

			for (_int i = 0; i < Desc["ColorSetCnt"].asInt(); ++i)
			{
				colorKeyFrame.fTime = fTime[i];
				colorKeyFrame.eEaseType = iEase[i];
				colorKeyFrame.vColorMin = vMin[i];
				colorKeyFrame.vColorMax = vMax[i];

				eParticleDesc.KeyFrames.ColorKeyFrames.push_back(colorKeyFrame);
			}
		}

		shared_ptr<CParticleSystem> pParticle = CParticleSystem::Create(&eParticleDesc);

		// 여기
		pEffectGroup->AddEffect(_strEffectID, pParticle);
	}

	return S_OK;
}

HRESULT CEffectMgr::AddAnimEffects()
{

	_int iAnimEffectCloneCnt;
	string strAnimEffectID;
	CAnimEffect::ANIMEFFECT_DESC eAnimEffectDesc;

	// Boss Earthquake Attack
	{
		vector<shared_ptr<CAnimEffect>> pAnimEffectVec;

		iAnimEffectCloneCnt = 3;
		strAnimEffectID = "Boss_EarthQuake_Anim";

		eAnimEffectDesc.strModelKey = "Crash_Rock";
		eAnimEffectDesc.StartTrackPos = 6;
		
		eAnimEffectDesc.bStartOnOwnerTrackPos = true;
		eAnimEffectDesc.StartOwnerTrackPos = 48;

		eAnimEffectDesc.vRotation = { 0.f, 0.f, 0.f };
		eAnimEffectDesc.bRotRelatedOwner = true;
		eAnimEffectDesc.bRotFollowOwner = false;
		eAnimEffectDesc.vScale = { 5.f, 1.f, 11.f };
		eAnimEffectDesc.vPosition = { 0.f, 0.f, 24.f };
		eAnimEffectDesc.bPosRelatedOwner = true;
		eAnimEffectDesc.bPosFollowOwner = false;
		eAnimEffectDesc.iPosType = 0;

		eAnimEffectDesc.bUseDistortion = true;
		eAnimEffectDesc.strDistortionKey = "Noise_N_3001";
		eAnimEffectDesc.fDistortionSpeed = 1.f;
		eAnimEffectDesc.fDistortionWeight = 1.f;

		eAnimEffectDesc.bUseGlow = false;
		eAnimEffectDesc.bUseGlowColor = false;
		eAnimEffectDesc.vGlowColor = { 0.f, 0.f, 0.f, 0.f };

		eAnimEffectDesc.bUseDslv = true;
		eAnimEffectDesc.iDslvType = 1;  // 0 : Dslv In, 1 : Dslv Out
		eAnimEffectDesc.fDslvStartTime = -1.f;
		eAnimEffectDesc.fDslvDuration = 0.5f;
		eAnimEffectDesc.fDslvThick = 1.f;
		eAnimEffectDesc.vDslvColor = { 0.1f, 0.1f, 0.1f, 1.f };
		eAnimEffectDesc.strDslvTexKey = "Noise_3003";

		for (_int i = 0; i < iAnimEffectCloneCnt; ++i)
		{
			shared_ptr<CAnimEffect> pAnimEffect = CAnimEffect::Create();
			pAnimEffect->SetAnimEffectDesc(eAnimEffectDesc);

			pAnimEffectVec.push_back(pAnimEffect);

			if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), pAnimEffect)))
				return E_FAIL;
		}

		m_AnimEffects.emplace(strAnimEffectID, pAnimEffectVec);
	}


	// Plr Gaia Crash
	{
		vector<shared_ptr<CAnimEffect>> pAnimEffectVec;

		iAnimEffectCloneCnt = 3;
		strAnimEffectID = "Plr_GaiaCrash";

		eAnimEffectDesc.strModelKey = "Gaia_Crush";
		eAnimEffectDesc.StartTrackPos = 0;

		eAnimEffectDesc.bStartOnOwnerTrackPos = true;
		eAnimEffectDesc.StartOwnerTrackPos = 30;

		eAnimEffectDesc.vRotation = { 0.f, 1.57f, 0.f };
		eAnimEffectDesc.bRotRelatedOwner = true;
		eAnimEffectDesc.bRotFollowOwner = false;
		eAnimEffectDesc.vScale = { 3.f, 3.f, 3.f };
		eAnimEffectDesc.vPosition = { 0.f, 0.f, 9.f };
		eAnimEffectDesc.iPosType = 0;
		eAnimEffectDesc.bPosRelatedOwner = true;
		eAnimEffectDesc.bPosFollowOwner = false;

		eAnimEffectDesc.bUseDistortion = true;
		eAnimEffectDesc.strDistortionKey = "Noise_N_3001";
		eAnimEffectDesc.fDistortionSpeed = 1.f;
		eAnimEffectDesc.fDistortionWeight = 1.f;

		eAnimEffectDesc.bUseGlow = false;
		eAnimEffectDesc.bUseGlowColor = false;
		eAnimEffectDesc.vGlowColor = { 0.f, 0.f, 0.f, 0.f };

		eAnimEffectDesc.bUseDslv = true;
		eAnimEffectDesc.iDslvType = 1;  // 0 : Dslv In, 1 : Dslv Out
		eAnimEffectDesc.fDslvStartTime = -1.f;
		eAnimEffectDesc.fDslvDuration = 0.5f;
		eAnimEffectDesc.fDslvThick = 1.f;
		eAnimEffectDesc.vDslvColor = { 0.1f, 0.1f, 0.1f, 1.f };
		eAnimEffectDesc.strDslvTexKey = "Noise_3003";

		for (_int i = 0; i < iAnimEffectCloneCnt; ++i)
		{
			shared_ptr<CAnimEffect> pAnimEffect = CAnimEffect::Create();
			pAnimEffect->SetAnimEffectDesc(eAnimEffectDesc);

			pAnimEffectVec.push_back(pAnimEffect);

			if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), pAnimEffect)))
				return E_FAIL;
		}

		m_AnimEffects.emplace(strAnimEffectID, pAnimEffectVec);
	}


	// Meteor Burst
	{
		vector<shared_ptr<CAnimEffect>> pAnimEffectVec;

		iAnimEffectCloneCnt = 3;
		strAnimEffectID = "Plr_Meteor_Burst_Anim";

		eAnimEffectDesc.strModelKey = "Crash_Rock";
		eAnimEffectDesc.StartTrackPos = 0;

		eAnimEffectDesc.bStartOnOwnerTrackPos = false;
		eAnimEffectDesc.StartOwnerTrackPos = 0;

		eAnimEffectDesc.vRotation = { 0.f, 0.f, 0.f };
		eAnimEffectDesc.bRotRelatedOwner = false;
		eAnimEffectDesc.bRotFollowOwner = false;
		eAnimEffectDesc.vScale = { 3.f, 3.f, 3.f };
		eAnimEffectDesc.vPosition = { 0.f, -5.f, 0.f };
		eAnimEffectDesc.bPosRelatedOwner = true;
		eAnimEffectDesc.bPosFollowOwner = true;
		eAnimEffectDesc.iPosType = 0;

		eAnimEffectDesc.bUseDistortion = false;
		eAnimEffectDesc.strDistortionKey = "Noise_N_3001";
		eAnimEffectDesc.fDistortionSpeed = 1.f;
		eAnimEffectDesc.fDistortionWeight = 1.f;

		eAnimEffectDesc.bUseGlow = false;
		eAnimEffectDesc.bUseGlowColor = false;
		eAnimEffectDesc.vGlowColor = { 0.f, 0.f, 0.f, 0.f };

		eAnimEffectDesc.bUseDslv = true;
		eAnimEffectDesc.iDslvType = 1;  // 0 : Dslv In, 1 : Dslv Out
		eAnimEffectDesc.fDslvStartTime = -1.f;
		eAnimEffectDesc.fDslvDuration = 0.5f;
		eAnimEffectDesc.fDslvThick = 1.f;
		eAnimEffectDesc.vDslvColor = { 0.1f, 0.1f, 0.1f, 1.f };
		eAnimEffectDesc.strDslvTexKey = "Noise_3003";

		for (_int i = 0; i < iAnimEffectCloneCnt; ++i)
		{
			shared_ptr<CAnimEffect> pAnimEffect = CAnimEffect::Create();
			pAnimEffect->SetAnimEffectDesc(eAnimEffectDesc);

			pAnimEffectVec.push_back(pAnimEffect);

			if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), pAnimEffect)))
				return E_FAIL;
		}

		m_AnimEffects.emplace(strAnimEffectID, pAnimEffectVec);
	}


	// Orc EarthQuake Attack
	{
		vector<shared_ptr<CAnimEffect>> pAnimEffectVec;

		iAnimEffectCloneCnt = 3;
		strAnimEffectID = "Orc_EarthQuake_Anim";

		eAnimEffectDesc.strModelKey = "Gaia_Crush";
		eAnimEffectDesc.StartTrackPos = 0;

		eAnimEffectDesc.bStartOnOwnerTrackPos = true;
		eAnimEffectDesc.StartOwnerTrackPos = 16.5;

		eAnimEffectDesc.vRotation = { 0.f, 1.57f, 0.f };
		eAnimEffectDesc.bRotRelatedOwner = true;
		eAnimEffectDesc.bRotFollowOwner = false;
		eAnimEffectDesc.vScale = { 7.f, 2.f, 7.f };
		eAnimEffectDesc.vPosition = { 0.f, 0.f, 20.f };
		eAnimEffectDesc.iPosType = 0;
		eAnimEffectDesc.bPosRelatedOwner = true;
		eAnimEffectDesc.bPosFollowOwner = false;

		eAnimEffectDesc.bUseDistortion = true;
		eAnimEffectDesc.strDistortionKey = "Noise_N_3001";
		eAnimEffectDesc.fDistortionSpeed = 1.f;
		eAnimEffectDesc.fDistortionWeight = 1.f;

		eAnimEffectDesc.bUseGlow = false;
		eAnimEffectDesc.bUseGlowColor = false;
		eAnimEffectDesc.vGlowColor = { 0.f, 0.f, 0.f, 0.f };

		eAnimEffectDesc.bUseDslv = true;
		eAnimEffectDesc.iDslvType = 1;  // 0 : Dslv In, 1 : Dslv Out
		eAnimEffectDesc.fDslvStartTime = -1.f;
		eAnimEffectDesc.fDslvDuration = 0.5f;
		eAnimEffectDesc.fDslvThick = 1.f;
		eAnimEffectDesc.vDslvColor = { 0.1f, 0.1f, 0.1f, 1.f };
		eAnimEffectDesc.strDslvTexKey = "Noise_3003";

		for (_int i = 0; i < iAnimEffectCloneCnt; ++i)
		{
			shared_ptr<CAnimEffect> pAnimEffect = CAnimEffect::Create();
			pAnimEffect->SetAnimEffectDesc(eAnimEffectDesc);

			pAnimEffectVec.push_back(pAnimEffect);

			if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), pAnimEffect)))
				return E_FAIL;
		}

		m_AnimEffects.emplace(strAnimEffectID, pAnimEffectVec);
	}

	return S_OK;
}
