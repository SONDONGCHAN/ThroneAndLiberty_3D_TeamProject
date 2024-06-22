/*Down & UpScalling Æ÷ÇÔ*/

#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

float g_fScreenWidth;
float g_fScreenHeight;

float g_fDOFRange = 300.f;
float g_fFocus = 20.f;

bool g_UsingDOF = true;

Texture2D g_BlurTexture;

Texture2D g_BackBufferTexture;
Texture2D g_DepthTexture;


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

float4 PS_DOF(PS_IN In) : SV_Target0
{ 
    
    float vDepth = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord).y * 1000.f;
    vector vBackBuffer = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vBlur = g_BlurTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    
    float4 vColor;
    
    
    vColor = vBackBuffer;
    
    if (g_UsingDOF)
    {
        
    
    
        if (vDepth >= g_fFocus)
        {
            float fDepthDiff = (vDepth - g_fFocus) / (1000.f - g_fFocus);
            vColor = lerp(vBackBuffer, vBlur, saturate(10.f * fDepthDiff));
        }
        else
        {
            vColor = vBackBuffer;
        }
    
    }
    else
    {
        vColor = vBackBuffer;
    }
    
    return vColor;

}

technique11	DefaultTechnique 
{
    pass DepthOfField //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_DOF();
    }

}


