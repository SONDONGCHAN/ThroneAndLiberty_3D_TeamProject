#include "Shader_Defines.hlsli"
#include "Shader_Function.hlsli"

// cpp에서 계산할 수 있는 애들은 최대한 거기서 계산해서 들고오자.
// PS 셰이더에서 계산하지 말자는 뜻
// ex) NoiseUVScroll (Speed * ActiveTime * NoiseWeight), vColor (보간 해서 가져와)
struct EffectMeshDesc
{
	float      fDuration;

	bool         bBillBoard;

	float2     Padding1;

	bool         bUseDiffuse;
	bool         bUseMask;
	bool         bUseNoise;
	bool         bUseColor;

	int         iDiffuseSamplerType;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
	int         iDiffuseUVType;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS
	float       fDiffuseDiscardValue;
	int         iDiffuseDiscardArea;     // 0 : Less Than / 1 : More Than

	float2      fDiffuseUVScroll;
	float2      iDiffuseUVAtlasCnt;

	float       fDiffuseUVAtlasSpeed;
	float3      Padding4;

	int         iMaskSamplerType;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
	int         iMaskUVType;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS
	float       fMaskDiscardValue;
	int         iMaskDiscardArea;     // 0 : Less Than / 1 : More Than

	float2      fMaskUVScroll;
	float2      iMaskUVAtlasCnt;

	float       fMaskUVAtlasSpeed;

	int         iNoiseSamplerType;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
	int         iNoiseUVType;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS
	float       fNoiseDiscardValue;
	int         iNoiseDiscardArea;     // 0 : Less Than / 1 : More Than

	float3     fPadding2;

	float2      fNoiseUVScroll;
	float2      iNoiseUVAtlasCnt;
	float       fNoiseUVAtlasSpeed;

	int         iCurColorSplitNum;

	float2      fPadding3;
	float4      vCurColorSplitter;

	float4      vCurColor1;
	float4      vCurColor2;
	float4      vCurColor3;
	float4      vCurColor4;


	// 셰이더용 가중치?
};

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_vCamPos;

texture2D g_NormalTexture;

texture2D	g_DiffuseTexture;
texture2D	g_MaskTexture;
texture2D	g_NoiseTexture;

texture2D	g_DistortionTexture;
float		g_fDistortionTimer, g_fDistortionSpeed, g_fDistortionWeight;

bool        g_bUseBlurColor;
float4      g_vBlurColor;

float2		g_vDiffuseUVPivot;
float2		g_vMaskUVPivot;
float2		g_vNoiseUVPivot;

// Dissolve
bool    g_bDslv = false;
float   g_fDslvValue = 0.f;
float   g_fDslvThick = 1.f;
float4  g_vDslvColor;
Texture2D   g_DslvTexture;

EffectMeshDesc g_EffectDesc;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float3 vTangent : TANGENT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
};

struct VS_SIMPLE_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

	return Out;
}

VS_SIMPLE_OUT VS_BLUR(VS_IN In)
{
	VS_SIMPLE_OUT Out = (VS_SIMPLE_OUT)0;

	matrix matWV, matVP, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matVP = mul(g_ViewMatrix, g_ProjMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/*vector vPosition = mul(vector(In.vPosition.xyz, 1.f), g_WorldMatrix);
	vPosition *= 2.f;*/
	//vPosition += float4(In.vNormal.xyz * 0.1f, 0.f);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

VS_SIMPLE_OUT VS_DISTORTION(VS_IN In)
{
	VS_SIMPLE_OUT Out = (VS_SIMPLE_OUT)0;

	matrix matWV, matVP, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matVP = mul(g_ViewMatrix, g_ProjMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/*vector vPosition = mul(vector(In.vPosition.xyz, 1.f), g_WorldMatrix);
	vPosition += float4(In.vNormal.xyz * 0.2f, 0.f);

	Out.vPosition = mul(vPosition, matVP);
	Out.vTexcoord = In.vTexcoord;*/

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}


VS_OUT VS_OUTLINE(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matVP, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matVP = mul(g_ViewMatrix, g_ProjMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vProjPos = mul(vector(In.vPosition, 1.f), matWVP);

	vector vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix) + (vNormal * 0.1f);

	Out.vPosition = mul(vPosition, matVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = vNormal;

	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);


	return Out;

}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
	vector vNormal : SV_TARGET1;
	vector vDepth : SV_TARGET2;
	vector vEmissive : SV_TARGET3;
};

struct PS_SIMPLE_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct PS_SIMPLE_OUT
{
	vector vColor : SV_TARGET0;
};

struct PS_OUTLINE_OUT
{
	vector vDepth : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector  vDiffuseTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector  vMaskTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector	vColorMaskTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector  vNoiseTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector  vColor = vector(0.f, 0.f, 0.f, 0.f);

	float2	vTexcoord = In.vTexcoord;

	float2	vDiffuseTexcoord = In.vTexcoord;
	float2  vNoiseTexcoord = In.vTexcoord;
	float2  vMaskTexcoord = In.vTexcoord;

	float   fAlpha = 1.f;

	bool    isDiscarded = false;

	// 메쉬 자르기 용
	if (g_EffectDesc.bUseMask)
	{
		// UV Type에 따른 옵션
		// UV Fix
		if (0 == g_EffectDesc.iMaskUVType)
		{
		}
		// UV Scroll
		else if (1 == g_EffectDesc.iMaskUVType)
		{
			vMaskTexcoord += g_EffectDesc.fMaskUVScroll;
			//vMaskTexcoord = In.vTexcoord + g_EffectDesc.fMaskUVScroll;
		}
		// UV Atlas
		else if (2 == g_EffectDesc.iMaskUVType)
		{
			vMaskTexcoord.x /= g_EffectDesc.iMaskUVAtlasCnt.x;
			vMaskTexcoord.x += g_vMaskUVPivot.x;

			vMaskTexcoord.y /= g_EffectDesc.iMaskUVAtlasCnt.y;
			vMaskTexcoord.y += g_vMaskUVPivot.y;
		}

		// Sampler Type에 따른 옵션
		// Sampler Linear
		if (0 == g_EffectDesc.iMaskSamplerType)
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearSampler, vMaskTexcoord);
		}
		// Sampler Mirror
		else if (1 == g_EffectDesc.iMaskSamplerType)
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearMirrorSampler, vMaskTexcoord);
		}
		// Sampler Clamp
		else if (2 == g_EffectDesc.iMaskSamplerType)
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearClampSampler, vMaskTexcoord);
		}
		// Sampler Border
		else
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearBorderSampler, vMaskTexcoord);
		}

		// Discard 옵션
		// Discard More than Value
		if (g_EffectDesc.iMaskDiscardArea)
		{
			if (vMaskTexture.r >= g_EffectDesc.fMaskDiscardValue || vMaskTexture.a >= g_EffectDesc.fMaskDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = 0 ~ g_EffectDesc.fMaskDiscardValue
			fAlpha = vMaskTexture.r / g_EffectDesc.fMaskDiscardValue;
		}

		// Discard Less than Value
		else {
			if (vMaskTexture.r <= g_EffectDesc.fMaskDiscardValue || vMaskTexture.a <= g_EffectDesc.fMaskDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = g_EffectDesc.fMaskDiscardValue ~ 1
			fAlpha = (vMaskTexture.r - g_EffectDesc.fMaskDiscardValue) / (1 - g_EffectDesc.fMaskDiscardValue);
		}
	}

	// color 흔들기 용
	if (g_EffectDesc.bUseNoise)
	{
		// UV Type에 따른 옵션
		// UV Fix
		if (0 == g_EffectDesc.iNoiseUVType)
		{
		}
		// UV Scroll
		else if (1 == g_EffectDesc.iNoiseUVType)
		{
			vNoiseTexcoord.x += g_EffectDesc.fNoiseUVScroll.x;
			vNoiseTexcoord.y += g_EffectDesc.fNoiseUVScroll.y;
			vNoiseTexcoord.x = lerp(In.vTexcoord.x, vNoiseTexcoord.x, 0.1);
			vNoiseTexcoord.y = lerp(In.vTexcoord.y, vNoiseTexcoord.y, 0.1);
		}
		// UV Atlas
		else if (2 == g_EffectDesc.iNoiseUVType)
		{
			vNoiseTexcoord.x /= g_EffectDesc.iNoiseUVAtlasCnt.x;
			vNoiseTexcoord.x += g_vNoiseUVPivot.x;

			vNoiseTexcoord.y /= g_EffectDesc.iNoiseUVAtlasCnt.y;
			vNoiseTexcoord.y += g_vNoiseUVPivot.y;
		}

		// Sampler Type에 따른 옵션
		// Sampler Linear
		if (0 == g_EffectDesc.iNoiseSamplerType)
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearSampler, vNoiseTexcoord);
		}
		// Sampler Mirror
		else if (1 == g_EffectDesc.iNoiseSamplerType)
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearMirrorSampler, vNoiseTexcoord);
		}
		// Sampler Clamp
		else if (2 == g_EffectDesc.iNoiseSamplerType)
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearClampSampler, vNoiseTexcoord);
		}
		// Sampler Border
		else
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearBorderSampler, vNoiseTexcoord);
		}

		vTexcoord = vNoiseTexture;
		vMaskTexcoord += vTexcoord;
		/*In.vTexcoord = vNoiseTexture;
		vDiffuseTexcoord = In.vTexcoord;*/
	}

	// 이걸 마스크 텍스처 추출 후에 할 지 전에 할 지 확인해보기 (노이즈 텍스처를 마스크 텍스처에도 적용?)
	// 노이즈 텍스처와 마스크 텍스처는 별도로 생각하자
	// 노이즈 텍스처 -> 패턴 만들고 일그러뜨리기
	// 마스크 텍스처 -> 구역 나눠서 discard

	// 디퓨즈 바르기
	if (g_EffectDesc.bUseDiffuse)
	{
		// UV Type에 따른 옵션
		// UV Fix
		if (0 == g_EffectDesc.iDiffuseUVType)
		{
		}
		// UV Scroll
		else if (1 == g_EffectDesc.iDiffuseUVType)
		{
			vTexcoord.x += g_EffectDesc.fDiffuseUVScroll.x;
			vTexcoord.y += g_EffectDesc.fDiffuseUVScroll.y;
			vTexcoord.x = lerp(In.vTexcoord.x, vTexcoord.x, 0.1);
			vTexcoord.y = lerp(In.vTexcoord.y, vTexcoord.y, 0.1);
		}
		// UV Atlas
        else if (2 == g_EffectDesc.iDiffuseUVType || 3 == g_EffectDesc.iDiffuseUVType)
		{
			vTexcoord.x /= g_EffectDesc.iDiffuseUVAtlasCnt.x;
			vTexcoord.x += g_vDiffuseUVPivot.x;

			vTexcoord.y /= g_EffectDesc.iDiffuseUVAtlasCnt.y;
			vTexcoord.y += g_vDiffuseUVPivot.y;
		}

		// Sampler Type에 따른 옵션
		// Sampler Linear
		if (0 == g_EffectDesc.iDiffuseSamplerType)
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearSampler, vTexcoord);
		}
		// Sampler Mirror
		else if (1 == g_EffectDesc.iDiffuseSamplerType)
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearMirrorSampler, vTexcoord);
		}
		// Sampler Clamp
		else if (2 == g_EffectDesc.iDiffuseSamplerType)
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearClampSampler, vTexcoord);
		}
		// Sampler Border
		else
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearBorderSampler, vTexcoord);
		}

		// Discard 옵션
		// Discard More than Value
		if (g_EffectDesc.iDiffuseDiscardArea)
		{
			if (vDiffuseTexture.a >= g_EffectDesc.fDiffuseDiscardValue || vDiffuseTexture.r >= g_EffectDesc.fDiffuseDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = 0 ~ g_EffectDesc.fDiffuseDiscardValue
			fAlpha = vDiffuseTexture.a / g_EffectDesc.fDiffuseDiscardValue;
		}

		// Discard Less than Value
		else {
			if (vDiffuseTexture.a <= g_EffectDesc.fDiffuseDiscardValue || vDiffuseTexture.r <= g_EffectDesc.fDiffuseDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = g_EffectDesc.fDiffuseDiscardValue ~ 1
			fAlpha = (vDiffuseTexture.a - g_EffectDesc.fDiffuseDiscardValue) / (1 - g_EffectDesc.fDiffuseDiscardValue);
		}
		
		vColor = vDiffuseTexture;
		vColor.a = fAlpha;

	}

	// 컬러 칠하기 (컬러 Split 기준은 마스크의 r값이기 때문에, 마스크 재샘플링하자.)
	if (g_EffectDesc.bUseColor)
	{
		vColorMaskTexture = g_MaskTexture.Sample(g_LinearSampler, vMaskTexcoord);

		if (0 == g_EffectDesc.iCurColorSplitNum)
		{
			vColor += vector(1.f, 1.f, 1.f, 1.f);
		}


		else if (1 == g_EffectDesc.iCurColorSplitNum)
		{
			if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.x)
			{
				vColor += g_EffectDesc.vCurColor1;
			}
		}

		else if (2 == g_EffectDesc.iCurColorSplitNum)
		{
			if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.x)
			{
				vColor += g_EffectDesc.vCurColor1;
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.y)
			{
				vColor += lerp(g_EffectDesc.vCurColor1, g_EffectDesc.vCurColor2, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.x) / (g_EffectDesc.vCurColorSplitter.y - g_EffectDesc.vCurColorSplitter.x));
			}
			else
			{
				vColor += g_EffectDesc.vCurColor2;
			}
		}

		else if (3 == g_EffectDesc.iCurColorSplitNum)
		{
			if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.x)
			{
				vColor += g_EffectDesc.vCurColor1;
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.y)
			{
				vColor += lerp(g_EffectDesc.vCurColor1, g_EffectDesc.vCurColor2, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.x) / (g_EffectDesc.vCurColorSplitter.y - g_EffectDesc.vCurColorSplitter.x));
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.z)
			{
				vColor += lerp(g_EffectDesc.vCurColor2, g_EffectDesc.vCurColor3, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.y) / (g_EffectDesc.vCurColorSplitter.z - g_EffectDesc.vCurColorSplitter.y));
			}
			else
			{
				vColor += g_EffectDesc.vCurColor3;
			}
		}

		else if (4 == g_EffectDesc.iCurColorSplitNum)
		{
			if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.x)
			{
				vColor += g_EffectDesc.vCurColor1;
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.y)
			{
				vColor += lerp(g_EffectDesc.vCurColor1, g_EffectDesc.vCurColor2, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.x) / (g_EffectDesc.vCurColorSplitter.y - g_EffectDesc.vCurColorSplitter.x));
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.z)
			{
				vColor += lerp(g_EffectDesc.vCurColor2, g_EffectDesc.vCurColor3, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.y) / (g_EffectDesc.vCurColorSplitter.z - g_EffectDesc.vCurColorSplitter.y));
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.w)
			{
				vColor += lerp(g_EffectDesc.vCurColor3, g_EffectDesc.vCurColor4, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.z) / (g_EffectDesc.vCurColorSplitter.w - g_EffectDesc.vCurColorSplitter.z));
			}
			else
			{
				vColor += g_EffectDesc.vCurColor4;
			}
		}
	}

	if (g_EffectDesc.bUseColor && g_EffectDesc.bUseDiffuse)
		vColor /= 2.f;

	if (!isDiscarded)
		Out.vColor = vColor;

	if (g_bDslv)
	{
		vector vDslvTexture = g_DslvTexture.Sample(g_LinearSampler, In.vTexcoord);
		vector vDslvDiffuse = Dissolve(vDslvTexture.r, g_fDslvValue - 0.1f, g_fDslvValue, g_fDslvThick * 20.f, g_vDslvColor, Out.vColor);

		if (vDslvDiffuse.a <= 0.f)
			discard;

		Out.vColor = vDslvDiffuse;
	}



	/*if (Out.vColor.g <= 0.f)
		discard;*/

	float3 vNormal = In.vNormal.xyz;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);

	return Out;
}

//PS_SIMPLE_OUT PS_BLUR(PS_SIMPLE_IN In)
//{
//	PS_SIMPLE_OUT Out = (PS_SIMPLE_OUT)0;
//
//	vector  vDiffuseTexture = vector(0.f, 0.f, 0.f, 0.f);
//	vector  vMaskTexture = vector(0.f, 0.f, 0.f, 0.f);
//	vector	vColorMaskTexture = vector(0.f, 0.f, 0.f, 0.f);
//	vector  vNoiseTexture = vector(0.f, 0.f, 0.f, 0.f);
//	vector  vColor = vector(0.f, 0.f, 0.f, 0.f);
//
//	float2	vTexcoord = In.vTexcoord;
//
//	float2	vDiffuseTexcoord = In.vTexcoord;
//	float2  vNoiseTexcoord = In.vTexcoord;
//	float2  vMaskTexcoord = In.vTexcoord;
//
//	float   fAlpha = 1.f;
//
//	bool    isDiscarded = false;
//
//	// 메쉬 자르기 용
//	if (g_EffectDesc.bUseMask)
//	{
//		// UV Type에 따른 옵션
//		// UV Fix
//		if (0 == g_EffectDesc.iMaskUVType)
//		{
//		}
//		// UV Scroll
//		else if (1 == g_EffectDesc.iMaskUVType)
//		{
//			vMaskTexcoord += g_EffectDesc.fMaskUVScroll;
//			//vMaskTexcoord = In.vTexcoord + g_EffectDesc.fMaskUVScroll;
//		}
//		// UV Atlas
//		else if (2 == g_EffectDesc.iMaskUVType)
//		{
//			vMaskTexcoord.x /= g_EffectDesc.iMaskUVAtlasCnt.x;
//			vMaskTexcoord.x += g_vMaskUVPivot.x;
//
//			vMaskTexcoord.y /= g_EffectDesc.iMaskUVAtlasCnt.y;
//			vMaskTexcoord.y += g_vMaskUVPivot.y;
//		}
//
//		// Sampler Type에 따른 옵션
//		// Sampler Linear
//		if (0 == g_EffectDesc.iMaskSamplerType)
//		{
//			vMaskTexture = g_MaskTexture.Sample(g_LinearSampler, vMaskTexcoord);
//		}
//		// Sampler Mirror
//		else if (1 == g_EffectDesc.iMaskSamplerType)
//		{
//			vMaskTexture = g_MaskTexture.Sample(g_LinearMirrorSampler, vMaskTexcoord);
//		}
//		// Sampler Clamp
//		else if (2 == g_EffectDesc.iMaskSamplerType)
//		{
//			vMaskTexture = g_MaskTexture.Sample(g_LinearClampSampler, vMaskTexcoord);
//		}
//		// Sampler Border
//		else
//		{
//			vMaskTexture = g_MaskTexture.Sample(g_LinearBorderSampler, vMaskTexcoord);
//		}
//
//		// Discard 옵션
//		// Discard More than Value
//		if (g_EffectDesc.iMaskDiscardArea)
//		{
//			if (vMaskTexture.r >= g_EffectDesc.fMaskDiscardValue || vMaskTexture.a >= g_EffectDesc.fMaskDiscardValue)
//			{
//				isDiscarded = true;
//				discard;
//			}
//
//			// Alpha = 0 ~ g_EffectDesc.fMaskDiscardValue
//			fAlpha = vMaskTexture.r / g_EffectDesc.fMaskDiscardValue;
//		}
//
//		// Discard Less than Value
//		else {
//			if (vMaskTexture.r <= g_EffectDesc.fMaskDiscardValue || vMaskTexture.a <= g_EffectDesc.fMaskDiscardValue)
//			{
//				isDiscarded = true;
//				discard;
//			}
//
//			// Alpha = g_EffectDesc.fMaskDiscardValue ~ 1
//			fAlpha = (vMaskTexture.r - g_EffectDesc.fMaskDiscardValue) / (1 - g_EffectDesc.fMaskDiscardValue);
//		}
//	}
//
//	// color 흔들기 용
//	if (g_EffectDesc.bUseNoise)
//	{
//		// UV Type에 따른 옵션
//		// UV Fix
//		if (0 == g_EffectDesc.iNoiseUVType)
//		{
//		}
//		// UV Scroll
//		else if (1 == g_EffectDesc.iNoiseUVType)
//		{
//			vNoiseTexcoord.x += g_EffectDesc.fNoiseUVScroll.x;
//			vNoiseTexcoord.y += g_EffectDesc.fNoiseUVScroll.y;
//			vNoiseTexcoord.x = lerp(In.vTexcoord.x, vNoiseTexcoord.x, 0.1);
//			vNoiseTexcoord.y = lerp(In.vTexcoord.y, vNoiseTexcoord.y, 0.1);
//		}
//		// UV Atlas
//		else if (2 == g_EffectDesc.iNoiseUVType)
//		{
//			vNoiseTexcoord.x /= g_EffectDesc.iNoiseUVAtlasCnt.x;
//			vNoiseTexcoord.x += g_vNoiseUVPivot.x;
//
//			vNoiseTexcoord.y /= g_EffectDesc.iNoiseUVAtlasCnt.y;
//			vNoiseTexcoord.y += g_vNoiseUVPivot.y;
//		}
//
//		// Sampler Type에 따른 옵션
//		// Sampler Linear
//		if (0 == g_EffectDesc.iNoiseSamplerType)
//		{
//			vNoiseTexture = g_NoiseTexture.Sample(g_LinearSampler, vNoiseTexcoord);
//		}
//		// Sampler Mirror
//		else if (1 == g_EffectDesc.iNoiseSamplerType)
//		{
//			vNoiseTexture = g_NoiseTexture.Sample(g_LinearMirrorSampler, vNoiseTexcoord);
//		}
//		// Sampler Clamp
//		else if (2 == g_EffectDesc.iNoiseSamplerType)
//		{
//			vNoiseTexture = g_NoiseTexture.Sample(g_LinearClampSampler, vNoiseTexcoord);
//		}
//		// Sampler Border
//		else
//		{
//			vNoiseTexture = g_NoiseTexture.Sample(g_LinearBorderSampler, vNoiseTexcoord);
//		}
//
//		vTexcoord = vNoiseTexture;
//		vMaskTexcoord += vTexcoord;
//		/*In.vTexcoord = vNoiseTexture;
//		vDiffuseTexcoord = In.vTexcoord;*/
//	}
//
//	// 이걸 마스크 텍스처 추출 후에 할 지 전에 할 지 확인해보기 (노이즈 텍스처를 마스크 텍스처에도 적용?)
//	// 노이즈 텍스처와 마스크 텍스처는 별도로 생각하자
//	// 노이즈 텍스처 -> 패턴 만들고 일그러뜨리기
//	// 마스크 텍스처 -> 구역 나눠서 discard
//
//	// 디퓨즈 바르기
//	if (g_EffectDesc.bUseDiffuse)
//	{
//		// UV Type에 따른 옵션
//		// UV Fix
//		if (0 == g_EffectDesc.iDiffuseUVType)
//		{
//		}
//		// UV Scroll
//		else if (1 == g_EffectDesc.iDiffuseUVType)
//		{
//			vTexcoord.x += g_EffectDesc.fDiffuseUVScroll.x;
//			vTexcoord.y += g_EffectDesc.fDiffuseUVScroll.y;
//			vTexcoord.x = lerp(In.vTexcoord.x, vTexcoord.x, 0.1);
//			vTexcoord.y = lerp(In.vTexcoord.y, vTexcoord.y, 0.1);
//		}
//		// UV Atlas
//		else if (2 == g_EffectDesc.iDiffuseUVType)
//		{
//			vTexcoord.x /= g_EffectDesc.iDiffuseUVAtlasCnt.x;
//			vTexcoord.x += g_vDiffuseUVPivot.x;
//
//			vTexcoord.y /= g_EffectDesc.iDiffuseUVAtlasCnt.y;
//			vTexcoord.y += g_vDiffuseUVPivot.y;
//		}
//
//		// Sampler Type에 따른 옵션
//		// Sampler Linear
//		if (0 == g_EffectDesc.iDiffuseSamplerType)
//		{
//			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearSampler, vTexcoord);
//		}
//		// Sampler Mirror
//		else if (1 == g_EffectDesc.iDiffuseSamplerType)
//		{
//			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearMirrorSampler, vTexcoord);
//		}
//		// Sampler Clamp
//		else if (2 == g_EffectDesc.iDiffuseSamplerType)
//		{
//			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearClampSampler, vTexcoord);
//		}
//		// Sampler Border
//		else
//		{
//			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearBorderSampler, vTexcoord);
//		}
//
//		// Discard 옵션
//		// Discard More than Value
//		if (g_EffectDesc.iDiffuseDiscardArea)
//		{
//			if (vDiffuseTexture.a >= g_EffectDesc.fDiffuseDiscardValue || vDiffuseTexture.r >= g_EffectDesc.fDiffuseDiscardValue)
//			{
//				isDiscarded = true;
//				discard;
//			}
//
//			// Alpha = 0 ~ g_EffectDesc.fDiffuseDiscardValue
//			fAlpha = vDiffuseTexture.a / g_EffectDesc.fDiffuseDiscardValue;
//		}
//
//		// Discard Less than Value
//		else {
//			if (vDiffuseTexture.a <= g_EffectDesc.fDiffuseDiscardValue || vDiffuseTexture.r <= g_EffectDesc.fDiffuseDiscardValue)
//			{
//				isDiscarded = true;
//				discard;
//			}
//
//			// Alpha = g_EffectDesc.fDiffuseDiscardValue ~ 1
//			fAlpha = (vDiffuseTexture.a - g_EffectDesc.fDiffuseDiscardValue) / (1 - g_EffectDesc.fDiffuseDiscardValue);
//		}
//
//		vColor = vDiffuseTexture;
//		vColor.a = fAlpha;
//
//	}
//
//	if (!isDiscarded)
//		Out.vColor = vColor;
//
//	if (g_bUseBlurColor)
//	{
//		Out.vColor = g_vBlurColor;
//	}
//
//	return Out;
//}

PS_OUT PS_BLUR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector  vDiffuseTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector  vMaskTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector	vColorMaskTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector  vNoiseTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector  vColor = vector(0.f, 0.f, 0.f, 0.f);

	float2	vTexcoord = In.vTexcoord;

	float2	vDiffuseTexcoord = In.vTexcoord;
	float2  vNoiseTexcoord = In.vTexcoord;
	float2  vMaskTexcoord = In.vTexcoord;

	float   fAlpha = 1.f;

	bool    isDiscarded = false;

	// 메쉬 자르기 용
	if (g_EffectDesc.bUseMask)
	{
		// UV Type에 따른 옵션
		// UV Fix
		if (0 == g_EffectDesc.iMaskUVType)
		{
		}
		// UV Scroll
		else if (1 == g_EffectDesc.iMaskUVType)
		{
			vMaskTexcoord += g_EffectDesc.fMaskUVScroll;
			//vMaskTexcoord = In.vTexcoord + g_EffectDesc.fMaskUVScroll;
		}
		// UV Atlas
		else if (2 == g_EffectDesc.iMaskUVType)
		{
			vMaskTexcoord.x /= g_EffectDesc.iMaskUVAtlasCnt.x;
			vMaskTexcoord.x += g_vMaskUVPivot.x;

			vMaskTexcoord.y /= g_EffectDesc.iMaskUVAtlasCnt.y;
			vMaskTexcoord.y += g_vMaskUVPivot.y;
		}

		// Sampler Type에 따른 옵션
		// Sampler Linear
		if (0 == g_EffectDesc.iMaskSamplerType)
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearSampler, vMaskTexcoord);
		}
		// Sampler Mirror
		else if (1 == g_EffectDesc.iMaskSamplerType)
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearMirrorSampler, vMaskTexcoord);
		}
		// Sampler Clamp
		else if (2 == g_EffectDesc.iMaskSamplerType)
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearClampSampler, vMaskTexcoord);
		}
		// Sampler Border
		else
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearBorderSampler, vMaskTexcoord);
		}

		// Discard 옵션
		// Discard More than Value
		if (g_EffectDesc.iMaskDiscardArea)
		{
			if (vMaskTexture.r >= g_EffectDesc.fMaskDiscardValue || vMaskTexture.a >= g_EffectDesc.fMaskDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = 0 ~ g_EffectDesc.fMaskDiscardValue
			fAlpha = vMaskTexture.r / g_EffectDesc.fMaskDiscardValue;
		}

		// Discard Less than Value
		else {
			if (vMaskTexture.r <= g_EffectDesc.fMaskDiscardValue || vMaskTexture.a <= g_EffectDesc.fMaskDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = g_EffectDesc.fMaskDiscardValue ~ 1
			fAlpha = (vMaskTexture.r - g_EffectDesc.fMaskDiscardValue) / (1 - g_EffectDesc.fMaskDiscardValue);
		}
	}

	// color 흔들기 용
	if (g_EffectDesc.bUseNoise)
	{
		// UV Type에 따른 옵션
		// UV Fix
		if (0 == g_EffectDesc.iNoiseUVType)
		{
		}
		// UV Scroll
		else if (1 == g_EffectDesc.iNoiseUVType)
		{
			vNoiseTexcoord.x += g_EffectDesc.fNoiseUVScroll.x;
			vNoiseTexcoord.y += g_EffectDesc.fNoiseUVScroll.y;
			vNoiseTexcoord.x = lerp(In.vTexcoord.x, vNoiseTexcoord.x, 0.1);
			vNoiseTexcoord.y = lerp(In.vTexcoord.y, vNoiseTexcoord.y, 0.1);
		}
		// UV Atlas
		else if (2 == g_EffectDesc.iNoiseUVType)
		{
			vNoiseTexcoord.x /= g_EffectDesc.iNoiseUVAtlasCnt.x;
			vNoiseTexcoord.x += g_vNoiseUVPivot.x;

			vNoiseTexcoord.y /= g_EffectDesc.iNoiseUVAtlasCnt.y;
			vNoiseTexcoord.y += g_vNoiseUVPivot.y;
		}

		// Sampler Type에 따른 옵션
		// Sampler Linear
		if (0 == g_EffectDesc.iNoiseSamplerType)
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearSampler, vNoiseTexcoord);
		}
		// Sampler Mirror
		else if (1 == g_EffectDesc.iNoiseSamplerType)
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearMirrorSampler, vNoiseTexcoord);
		}
		// Sampler Clamp
		else if (2 == g_EffectDesc.iNoiseSamplerType)
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearClampSampler, vNoiseTexcoord);
		}
		// Sampler Border
		else
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearBorderSampler, vNoiseTexcoord);
		}

		vTexcoord = vNoiseTexture;
		vMaskTexcoord += vTexcoord;
		/*In.vTexcoord = vNoiseTexture;
		vDiffuseTexcoord = In.vTexcoord;*/
	}

	// 이걸 마스크 텍스처 추출 후에 할 지 전에 할 지 확인해보기 (노이즈 텍스처를 마스크 텍스처에도 적용?)
	// 노이즈 텍스처와 마스크 텍스처는 별도로 생각하자
	// 노이즈 텍스처 -> 패턴 만들고 일그러뜨리기
	// 마스크 텍스처 -> 구역 나눠서 discard

	// 디퓨즈 바르기
	if (g_EffectDesc.bUseDiffuse)
	{
		// UV Type에 따른 옵션
		// UV Fix
		if (0 == g_EffectDesc.iDiffuseUVType)
		{
		}
		// UV Scroll
		else if (1 == g_EffectDesc.iDiffuseUVType)
		{
			vTexcoord.x += g_EffectDesc.fDiffuseUVScroll.x;
			vTexcoord.y += g_EffectDesc.fDiffuseUVScroll.y;
			vTexcoord.x = lerp(In.vTexcoord.x, vTexcoord.x, 0.1);
			vTexcoord.y = lerp(In.vTexcoord.y, vTexcoord.y, 0.1);
		}
		// UV Atlas
		else if (2 == g_EffectDesc.iDiffuseUVType || 3 == g_EffectDesc.iDiffuseUVType)
		{
			vTexcoord.x /= g_EffectDesc.iDiffuseUVAtlasCnt.x;
			vTexcoord.x += g_vDiffuseUVPivot.x;

			vTexcoord.y /= g_EffectDesc.iDiffuseUVAtlasCnt.y;
			vTexcoord.y += g_vDiffuseUVPivot.y;
		}

		// Sampler Type에 따른 옵션
		// Sampler Linear
		if (0 == g_EffectDesc.iDiffuseSamplerType)
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearSampler, vTexcoord);
		}
		// Sampler Mirror
		else if (1 == g_EffectDesc.iDiffuseSamplerType)
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearMirrorSampler, vTexcoord);
		}
		// Sampler Clamp
		else if (2 == g_EffectDesc.iDiffuseSamplerType)
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearClampSampler, vTexcoord);
		}
		// Sampler Border
		else
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearBorderSampler, vTexcoord);
		}

		// Discard 옵션
		// Discard More than Value
		if (g_EffectDesc.iDiffuseDiscardArea)
		{
			if (vDiffuseTexture.a >= g_EffectDesc.fDiffuseDiscardValue || vDiffuseTexture.r >= g_EffectDesc.fDiffuseDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = 0 ~ g_EffectDesc.fDiffuseDiscardValue
			fAlpha = vDiffuseTexture.a / g_EffectDesc.fDiffuseDiscardValue;
		}

		// Discard Less than Value
		else {
			if (vDiffuseTexture.a <= g_EffectDesc.fDiffuseDiscardValue || vDiffuseTexture.r <= g_EffectDesc.fDiffuseDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = g_EffectDesc.fDiffuseDiscardValue ~ 1
			fAlpha = (vDiffuseTexture.a - g_EffectDesc.fDiffuseDiscardValue) / (1 - g_EffectDesc.fDiffuseDiscardValue);
		}

		vColor = vDiffuseTexture;
		vColor.a = fAlpha;

	}

	// 컬러 칠하기 (컬러 Split 기준은 마스크의 r값이기 때문에, 마스크 재샘플링하자.)
	if (g_EffectDesc.bUseColor)
	{
		vColorMaskTexture = g_MaskTexture.Sample(g_LinearSampler, vMaskTexcoord);

		if (0 == g_EffectDesc.iCurColorSplitNum)
		{
			vColor += vector(1.f, 1.f, 1.f, 1.f);
		}


		else if (1 == g_EffectDesc.iCurColorSplitNum)
		{
			if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.x)
			{
				vColor += g_EffectDesc.vCurColor1;
			}
		}

		else if (2 == g_EffectDesc.iCurColorSplitNum)
		{
			if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.x)
			{
				vColor += g_EffectDesc.vCurColor1;
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.y)
			{
				vColor += lerp(g_EffectDesc.vCurColor1, g_EffectDesc.vCurColor2, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.x) / (g_EffectDesc.vCurColorSplitter.y - g_EffectDesc.vCurColorSplitter.x));
			}
			else
			{
				vColor += g_EffectDesc.vCurColor2;
			}
		}

		else if (3 == g_EffectDesc.iCurColorSplitNum)
		{
			if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.x)
			{
				vColor += g_EffectDesc.vCurColor1;
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.y)
			{
				vColor += lerp(g_EffectDesc.vCurColor1, g_EffectDesc.vCurColor2, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.x) / (g_EffectDesc.vCurColorSplitter.y - g_EffectDesc.vCurColorSplitter.x));
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.z)
			{
				vColor += lerp(g_EffectDesc.vCurColor2, g_EffectDesc.vCurColor3, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.y) / (g_EffectDesc.vCurColorSplitter.z - g_EffectDesc.vCurColorSplitter.y));
			}
			else
			{
				vColor += g_EffectDesc.vCurColor3;
			}
		}

		else if (4 == g_EffectDesc.iCurColorSplitNum)
		{
			if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.x)
			{
				vColor += g_EffectDesc.vCurColor1;
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.y)
			{
				vColor += lerp(g_EffectDesc.vCurColor1, g_EffectDesc.vCurColor2, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.x) / (g_EffectDesc.vCurColorSplitter.y - g_EffectDesc.vCurColorSplitter.x));
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.z)
			{
				vColor += lerp(g_EffectDesc.vCurColor2, g_EffectDesc.vCurColor3, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.y) / (g_EffectDesc.vCurColorSplitter.z - g_EffectDesc.vCurColorSplitter.y));
			}
			else if (vColorMaskTexture.r <= g_EffectDesc.vCurColorSplitter.w)
			{
				vColor += lerp(g_EffectDesc.vCurColor3, g_EffectDesc.vCurColor4, (vColorMaskTexture.r - g_EffectDesc.vCurColorSplitter.z) / (g_EffectDesc.vCurColorSplitter.w - g_EffectDesc.vCurColorSplitter.z));
			}
			else
			{
				vColor += g_EffectDesc.vCurColor4;
			}
		}
	}

	if (g_EffectDesc.bUseColor && g_EffectDesc.bUseDiffuse)
		vColor /= 2.f;

	if (!isDiscarded)
		Out.vColor = vColor;

	if (g_bUseBlurColor)
	{
		Out.vColor = g_vBlurColor;
		if (Out.vColor.a >= vColor.a)
			Out.vColor.a = vColor.a;
	}

	if (g_bDslv)
	{
		vector vDslvTexture = g_DslvTexture.Sample(g_LinearSampler, In.vTexcoord);
		vector vDslvDiffuse = Dissolve(vDslvTexture.r, g_fDslvValue - 0.1f, g_fDslvValue, g_fDslvThick * 20.f, g_vDslvColor, Out.vColor);

		if (vDslvDiffuse.a <= 0.f)
			discard;

		Out.vColor = vDslvDiffuse;
	}

	float3 vNormal = In.vNormal.xyz;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_SIMPLE_OUT PS_DISTORTION(PS_SIMPLE_IN In)
{
	PS_SIMPLE_OUT Out = (PS_SIMPLE_OUT)0;

	vector  vDiffuseTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector  vMaskTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector	vColorMaskTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector  vNoiseTexture = vector(0.f, 0.f, 0.f, 0.f);
	vector  vColor = vector(0.f, 0.f, 0.f, 0.f);

	float2	vTexcoord = In.vTexcoord;

	float2	vDiffuseTexcoord = In.vTexcoord;
	float2  vNoiseTexcoord = In.vTexcoord;
	float2  vMaskTexcoord = In.vTexcoord;

	float   fAlpha = 1.f;

	bool    isDiscarded = false;

	// 메쉬 자르기 용
	if (g_EffectDesc.bUseMask)
	{
		// UV Type에 따른 옵션
		// UV Fix
		if (0 == g_EffectDesc.iMaskUVType)
		{
		}
		// UV Scroll
		else if (1 == g_EffectDesc.iMaskUVType)
		{
			vMaskTexcoord += g_EffectDesc.fMaskUVScroll;
			//vMaskTexcoord = In.vTexcoord + g_EffectDesc.fMaskUVScroll;
		}
		// UV Atlas
		else if (2 == g_EffectDesc.iMaskUVType)
		{
			vMaskTexcoord.x /= g_EffectDesc.iMaskUVAtlasCnt.x;
			vMaskTexcoord.x += g_vMaskUVPivot.x;

			vMaskTexcoord.y /= g_EffectDesc.iMaskUVAtlasCnt.y;
			vMaskTexcoord.y += g_vMaskUVPivot.y;
		}

		// Sampler Type에 따른 옵션
		// Sampler Linear
		if (0 == g_EffectDesc.iMaskSamplerType)
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearSampler, vMaskTexcoord);
		}
		// Sampler Mirror
		else if (1 == g_EffectDesc.iMaskSamplerType)
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearMirrorSampler, vMaskTexcoord);
		}
		// Sampler Clamp
		else if (2 == g_EffectDesc.iMaskSamplerType)
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearClampSampler, vMaskTexcoord);
		}
		// Sampler Border
		else
		{
			vMaskTexture = g_MaskTexture.Sample(g_LinearBorderSampler, vMaskTexcoord);
		}

		// Discard 옵션
		// Discard More than Value
		if (g_EffectDesc.iMaskDiscardArea)
		{
			if (vMaskTexture.r >= g_EffectDesc.fMaskDiscardValue || vMaskTexture.a >= g_EffectDesc.fMaskDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = 0 ~ g_EffectDesc.fMaskDiscardValue
			fAlpha = vMaskTexture.r / g_EffectDesc.fMaskDiscardValue;
		}

		// Discard Less than Value
		else {
			if (vMaskTexture.r <= g_EffectDesc.fMaskDiscardValue || vMaskTexture.a <= g_EffectDesc.fMaskDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = g_EffectDesc.fMaskDiscardValue ~ 1
			fAlpha = (vMaskTexture.r - g_EffectDesc.fMaskDiscardValue) / (1 - g_EffectDesc.fMaskDiscardValue);
		}
	}

	// color 흔들기 용
	if (g_EffectDesc.bUseNoise)
	{
		// UV Type에 따른 옵션
		// UV Fix
		if (0 == g_EffectDesc.iNoiseUVType)
		{
		}
		// UV Scroll
		else if (1 == g_EffectDesc.iNoiseUVType)
		{
			vNoiseTexcoord.x += g_EffectDesc.fNoiseUVScroll.x;
			vNoiseTexcoord.y += g_EffectDesc.fNoiseUVScroll.y;
			vNoiseTexcoord.x = lerp(In.vTexcoord.x, vNoiseTexcoord.x, 0.1);
			vNoiseTexcoord.y = lerp(In.vTexcoord.y, vNoiseTexcoord.y, 0.1);
		}
		// UV Atlas
		else if (2 == g_EffectDesc.iNoiseUVType)
		{
			vNoiseTexcoord.x /= g_EffectDesc.iNoiseUVAtlasCnt.x;
			vNoiseTexcoord.x += g_vNoiseUVPivot.x;

			vNoiseTexcoord.y /= g_EffectDesc.iNoiseUVAtlasCnt.y;
			vNoiseTexcoord.y += g_vNoiseUVPivot.y;
		}

		// Sampler Type에 따른 옵션
		// Sampler Linear
		if (0 == g_EffectDesc.iNoiseSamplerType)
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearSampler, vNoiseTexcoord);
		}
		// Sampler Mirror
		else if (1 == g_EffectDesc.iNoiseSamplerType)
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearMirrorSampler, vNoiseTexcoord);
		}
		// Sampler Clamp
		else if (2 == g_EffectDesc.iNoiseSamplerType)
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearClampSampler, vNoiseTexcoord);
		}
		// Sampler Border
		else
		{
			vNoiseTexture = g_NoiseTexture.Sample(g_LinearBorderSampler, vNoiseTexcoord);
		}

		vTexcoord = vNoiseTexture;
		vMaskTexcoord += vTexcoord;
		/*In.vTexcoord = vNoiseTexture;
		vDiffuseTexcoord = In.vTexcoord;*/
	}

	// 이걸 마스크 텍스처 추출 후에 할 지 전에 할 지 확인해보기 (노이즈 텍스처를 마스크 텍스처에도 적용?)
	// 노이즈 텍스처와 마스크 텍스처는 별도로 생각하자
	// 노이즈 텍스처 -> 패턴 만들고 일그러뜨리기
	// 마스크 텍스처 -> 구역 나눠서 discard

	// 디퓨즈 바르기
	if (g_EffectDesc.bUseDiffuse)
	{
		// UV Type에 따른 옵션
		// UV Fix
		if (0 == g_EffectDesc.iDiffuseUVType)
		{
		}
		// UV Scroll
		else if (1 == g_EffectDesc.iDiffuseUVType)
		{
			vTexcoord.x += g_EffectDesc.fDiffuseUVScroll.x;
			vTexcoord.y += g_EffectDesc.fDiffuseUVScroll.y;
			vTexcoord.x = lerp(In.vTexcoord.x, vTexcoord.x, 0.1);
			vTexcoord.y = lerp(In.vTexcoord.y, vTexcoord.y, 0.1);
		}
		// UV Atlas
        else if (2 == g_EffectDesc.iDiffuseUVType || 3 == g_EffectDesc.iDiffuseUVType)
		{
			vTexcoord.x /= g_EffectDesc.iDiffuseUVAtlasCnt.x;
			vTexcoord.x += g_vDiffuseUVPivot.x;

			vTexcoord.y /= g_EffectDesc.iDiffuseUVAtlasCnt.y;
			vTexcoord.y += g_vDiffuseUVPivot.y;
		}

		// Sampler Type에 따른 옵션
		// Sampler Linear
		if (0 == g_EffectDesc.iDiffuseSamplerType)
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearSampler, vTexcoord);
		}
		// Sampler Mirror
		else if (1 == g_EffectDesc.iDiffuseSamplerType)
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearMirrorSampler, vTexcoord);
		}
		// Sampler Clamp
		else if (2 == g_EffectDesc.iDiffuseSamplerType)
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearClampSampler, vTexcoord);
		}
		// Sampler Border
		else
		{
			vDiffuseTexture = g_DiffuseTexture.Sample(g_LinearBorderSampler, vTexcoord);
		}

		// Discard 옵션
		// Discard More than Value
		if (g_EffectDesc.iDiffuseDiscardArea)
		{
			if (vDiffuseTexture.a >= g_EffectDesc.fDiffuseDiscardValue || vDiffuseTexture.r >= g_EffectDesc.fDiffuseDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = 0 ~ g_EffectDesc.fDiffuseDiscardValue
			fAlpha = vDiffuseTexture.a / g_EffectDesc.fDiffuseDiscardValue;
		}

		// Discard Less than Value
		else {
			if (vDiffuseTexture.a <= g_EffectDesc.fDiffuseDiscardValue || vDiffuseTexture.r <= g_EffectDesc.fDiffuseDiscardValue)
			{
				isDiscarded = true;
				discard;
			}

			// Alpha = g_EffectDesc.fDiffuseDiscardValue ~ 1
			fAlpha = (vDiffuseTexture.a - g_EffectDesc.fDiffuseDiscardValue) / (1 - g_EffectDesc.fDiffuseDiscardValue);
		}

		vColor = vDiffuseTexture;
		vColor.a = fAlpha;

	}

	if (!isDiscarded)
		Out.vColor = vColor;

	vector	vDistortionTexture = g_DistortionTexture.Sample(g_LinearSampler, In.vTexcoord + g_fDistortionTimer * g_fDistortionSpeed);

	Out.vColor = vDistortionTexture;
	Out.vColor.a = g_fDistortionWeight;

	return Out;
}

PS_OUTLINE_OUT PS_OUTLINE(PS_IN In)
{
	PS_OUTLINE_OUT Out = (PS_OUTLINE_OUT)0;

	vector vARMDesc = { 1.f, 0.f, 0.f, 0.f };
	/*ARM 텍스쳐(Light 연산) 적용 여부를 위해 저장해두는 변수입니다*/
	float fDepthARM = 0.f;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, fDepthARM, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Default // 0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		ComputeShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Blur // 1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		ComputeShader = NULL;
		PixelShader = compile ps_5_0 PS_BLUR();
	}

	pass Distortion	// 2
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_DISTORTION();
		GeometryShader = NULL;
		ComputeShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();
	}

	pass Outline // 3
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_OUTLINE();
		GeometryShader = NULL;
		ComputeShader = NULL;
		PixelShader = compile ps_5_0 PS_OUTLINE();
	}
}