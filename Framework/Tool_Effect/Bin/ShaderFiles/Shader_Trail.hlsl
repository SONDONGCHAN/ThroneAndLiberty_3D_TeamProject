#include "Shader_Defines.hlsli"

matrix	g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3	g_vCamPos;

texture2D	g_DiffuseTexture;
texture2D	g_MaskTexture;

float		g_fAlpha = 1.f;
float4		g_vUpColor = vector(1.f, 1.f, 1.f, 1.f);
float4		g_vDownColor = vector(1.f, 1.f, 1.f, 1.f);

float		g_fLifeTime = 0.f;
float		g_fDuration = 0.f;

texture2D	g_DistortionTexture;
float		g_fDistortionTimer, g_fDistortionSpeed, g_fDistortionWeight;

float4      g_vBlurColor;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}
struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (0.9 > Out.vColor.r)
		discard;

	vector vMaskColor = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);

	// 생성되는 시간
	if (g_fLifeTime > g_fDuration - 0.1f)
	{
		if (vMaskColor.r > (g_fDuration - g_fLifeTime) * 10.f)
			discard;
	}

	// 사라지는 시간
	else if (g_fLifeTime < 0.1f)
	{
		if (vMaskColor.r < (0.1f - g_fLifeTime) * 10.f)
			discard;
	}

	Out.vColor.a = Out.vColor.r;

	if (Out.vColor.a <= 0.3)
		discard;

	Out.vColor.gb = Out.vColor.r;

	return Out;
}

PS_OUT PS_BLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (Out.vColor.r < 0.1f)
		discard;

	Out.vColor = g_vDownColor * (1 - Out.vColor.r) + g_vUpColor * Out.vColor.r;

	Out.vColor.a *= In.vTexcoord.x;

	Out.vColor.a *= g_fAlpha;
	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (Out.vColor.r < 0.1f)
		discard;

	Out.vColor = g_vDownColor * (1 - Out.vColor.r) + g_vUpColor * Out.vColor.r;

	Out.vColor.a *= In.vTexcoord.x;

	Out.vColor.a *= g_fAlpha;
	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vDistortionTexture = g_DistortionTexture.Sample(g_LinearSampler, In.vTexcoord + g_fDistortionTimer * g_fDistortionSpeed);

	Out.vColor = vDistortionTexture;
	Out.vColor.a = g_fDistortionWeight;

	return Out;
}

technique11	DefaultTechnique
{
	pass Default
	{
		/* RenderState설정. */
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Trail
	{
		/* RenderState설정. */
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TRAIL();
	}

	pass Blur
	{
		/* RenderState설정. */
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLUR();
	}

	pass Distortion
	{
		/* RenderState설정. */
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();
	}
}
