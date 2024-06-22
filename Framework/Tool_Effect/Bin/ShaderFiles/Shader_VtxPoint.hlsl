#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_TargetWorldMatrix;
float       g_fThickness;
vector      g_vColor;

bool        g_bUseGlowColor = true;
float4      g_vGlowColor = { 1.f, 1.f, 1.f, 1.f };
Texture2D   g_DiffuseTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition        : POSITION0;
    float4 vPosition_Target    : POSITION1;
    
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    Out.vPosition       = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition_Target = mul(vector(In.vPosition, 1.f), g_TargetWorldMatrix);
    
    return Out;
}


struct GS_IN
{
    float4 vPosition        : POSITION0;
    float4 vPosition_Target : POSITION1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    float3 vLook = (In[0].vPosition_Target - In[0].vPosition).xyz;
    float3 vRight   = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) ;
    float3 vUp      = normalize(cross(vLook, vRight));
	matrix matVP    = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition - float4(vRight * g_fThickness, 0.f) + float4(vLook, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);

    Out[1].vPosition = In[0].vPosition + float4(vRight * g_fThickness, 0.f) + float4(vLook, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);

    Out[2].vPosition = In[0].vPosition + float4(vRight * g_fThickness, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);

    Out[3].vPosition = In[0].vPosition - float4(vRight * g_fThickness, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);

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
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD0;    
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

struct PS_GLOW_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //vector vMtrlDiffuse = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
    
    //if (vMtrlDiffuse.a < 0.01f)
    //    discard;
        
    //float4 vColor;
    
    //if (vMtrlDiffuse.a < 0.8f)
    //    vColor.rgb = g_vColor.rgb * (1.f - vMtrlDiffuse.a);
    //else
    //    vColor.rgb = vMtrlDiffuse.rgb;
    
    //vColor.a = g_vColor.a;
    
    //Out.vColor = vColor;
    
    Out.vColor = g_vColor;
    
    //Out.vColor = vector(113.f/255.f, 199.f/255.f, 236.f/255.f, g_fAlpha);
  
    return Out;
}

PS_GLOW_OUT PS_GLOW(PS_IN In)
{
    PS_GLOW_OUT Out = (PS_GLOW_OUT) 0;

    if (g_bUseGlowColor)
    {
        Out.vColor = g_vGlowColor;
    }
    else
    {
        vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

        if (vMtrlDiffuse.a < 0.1f)
            discard;

        Out.vColor = vMtrlDiffuse;
    }

    return Out;
}

/* EffectFramework */

technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        //SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass Glow // 1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_GLOW();
    }

}


