#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vCamPosition;

texture2D g_DiffuseTexture;
float g_fAlphaDiscard;

bool	g_bUseColor;

bool    g_bUseBlurColor;
float4  g_vBlurColor;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vPSize : PSIZE;

	row_major float4x4 TransformMatrix : WORLD;

	float4 vColor : COLOR0;
	float4 vPivot : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
	float4 vColor : COLOR0;
	float4 vPivot : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = float2(length(In.TransformMatrix._11_12_13) * In.vPSize.x,
		length(In.TransformMatrix._21_22_23) * In.vPSize.y);
	Out.vColor = In.vColor;
	Out.vPivot = mul(In.vPivot, g_WorldMatrix);

	return Out;
}

VS_OUT VS_GLOW(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	In.vPSize *= 1.3f;
	Out.vPSize = float2(length(In.TransformMatrix._11_12_13) * In.vPSize.x,
		length(In.TransformMatrix._21_22_23) * In.vPSize.y);
	//Out.vPSize *= 1.3f;
	Out.vColor = In.vColor;
	Out.vPivot = mul(In.vPivot, g_WorldMatrix);

	return Out;
}

/* Geometry Shader -> Creates Vertices
Called depending on the primitive you want to draw.
- Point: Once per Vertex.
- Line: Twice Vertex.
- Triangle: 3 times Vertex */
struct GS_IN
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
	float4 vColor : COLOR0;
	float4 vPivot : TEXCOORD0;
};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vColor : COLOR0;
};

[maxvertexcount(20)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
	GS_OUT		Out[4];

	float3		vLook = (g_vCamPosition - In[0].vPosition).xyz;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vColor = In[0].vColor;


	Out[1].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vColor = In[0].vColor;

	Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vColor = In[0].vColor;

	Out[3].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vColor = In[0].vColor;



	DataStream.Append(Out[0]);
	DataStream.Append(Out[1]);
	DataStream.Append(Out[2]);
	DataStream.RestartStrip();

	DataStream.Append(Out[0]);
	DataStream.Append(Out[2]);
	DataStream.Append(Out[3]);
}

[maxvertexcount(20)]
void GS_DIRECTIONAL(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
	GS_OUT		Out[4];

	float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
	/* 이 축이 바뀌어야 하는데? */
	float3 vUp = normalize((In[0].vPosition - In[0].vPivot).xyz) * In[0].vPSize.y * 0.5f;

	float3 vRight = normalize(cross(vUp, vLook)) * In[0].vPSize.x * 0.5f;

	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vColor = In[0].vColor;


	Out[1].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vColor = In[0].vColor;

	Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vColor = In[0].vColor;

	Out[3].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vColor = In[0].vColor;



	DataStream.Append(Out[0]);
	DataStream.Append(Out[1]);
	DataStream.Append(Out[2]);
	DataStream.RestartStrip();

	DataStream.Append(Out[0]);
	DataStream.Append(Out[2]);
	DataStream.Append(Out[3]);
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vColor : COLOR0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	// 수명이 다한 애들
	if (In.vColor.a <= 0.f)
		discard;

	/*Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (g_bUseColor)
	{
		Out.vColor.a *= (Out.vColor.r);

		if (In.vColor.a <= 0.f)
			discard;

		Out.vColor.rgb *= In.vColor;
		Out.vColor.rgb /= Out.vColor.a;
	}

	Out.vColor.a = In.vColor.a;

	if (Out.vColor.a < 0.3f)
		discard;*/

	//PS_OUT Out = (PS_OUT)0;

	// 디퓨즈 추출
	vector vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	// 너무 검거나 투명한 부분은 제거
	if (g_bUseColor)
    {
        if (vColor.r <= 0.3f || vColor.a <= 0.2f)
            discard;

		// 내가 설정해준 파티클 컬러
        Out.vColor = In.vColor;

        Out.vColor.a *= vColor.r;
        Out.vColor.a *= vColor.a;
    }
	// 컬러 설정 안했으면 텍스처 컬러
	else
	{
        if ((vColor.r <= 0.2f && vColor.g <= 0.2f && vColor.b <= 0.2f) || vColor.a <= 0.2f)
            discard;
		
            Out.vColor = vColor;
    }

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

PS_OUT PS_GLOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	// 수명이 다한 애들
	if (In.vColor.a <= 0.f)
		discard;

	// 디퓨즈 추출
	vector vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	// 너무 검은 부분은 제거
    if (vColor.r <= 0.3f || vColor.a <= 0.2f)
		discard;

	// 내가 설정해준 파티클 글로우 컬러
	if (g_bUseBlurColor)
	{
		Out.vColor = g_vBlurColor;
	}
	// 컬러 설정 안했으면 텍스처 컬러
	else
	{
		Out.vColor = vColor;
	}

	Out.vColor.a *= vColor.r;

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Blur // 1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_GLOW();
	}

	pass Directional
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DIRECTIONAL();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass DirectionalBlur
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DIRECTIONAL();
		PixelShader = compile ps_5_0 PS_GLOW();
	}
}