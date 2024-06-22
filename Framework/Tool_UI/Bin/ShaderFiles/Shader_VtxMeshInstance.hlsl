#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_vCamPos;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_ARMTexture;
Texture2D g_EmissionTexture;
Texture2D g_AOTexture;

float4x4 g_CascadeProjMat[3];
float4x4 g_CascadeViewMat[3];

bool g_IsUsingNormal, g_IsUsingARM, g_IsUsingEmission, g_IsUsingAO;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    row_major float4x4 TransformMatrix : WORLD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    //float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
       
    /*정점 생성 때 인스턴스 버퍼에 저장해둔 라업룩트랜스로 월드 매트릭스 생성*/
    //matrix TransformMatrix;
    //TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition;
    vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    //Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);
   

    return Out;

}


VS_OUT VS_CASCADE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    vector vPosition;
    vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
    Out.vPosition = mul(vPosition, g_WorldMatrix);
   
    return Out;

}

struct GS_IN
{
    float4 vPosition : SV_POSITION;
    
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};



[maxvertexcount(9)]
void GS_SHADOW(triangle GS_IN Input[3], inout TriangleStream<GS_OUT> DataStream)
{
    for (int iFace = 0; iFace < 3; iFace++)
    {
    
        GS_OUT Output;
        Output.RTIndex = iFace;
        
        for (int v = 0; v < 3; v++)
        {
            float4 vPosView = mul(Input[v].vPosition, g_CascadeViewMat[iFace]);
            Output.vPosition = mul(vPosView, g_CascadeProjMat[iFace]);
            DataStream.Append(Output);
        }
    
        DataStream.RestartStrip();   
    }
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    //float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vARM : SV_TARGET3;
    
};

//    vector vDepth : SV_TARGET2;


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

   
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    float3 vNormal;
    
    //노말 텍스쳐 O
    if (g_IsUsingNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(g_LinearSampler, In.vTexcoord);

        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        vNormal = mul(vNormal, WorldMatrix);        
    }
    //노말 텍스쳐 X
    else
    {
        vNormal = In.vNormal.xyz;

    }
    
    vector vARMDesc = { 1.f, 0.f, 0.f, 0.f };
    /*ARM 텍스쳐(Light 연산) 적용 여부를 위해 저장해두는 변수입니다*/
    float fDepthARM = 0.f;
    
    //ARM 텍스쳐 O
    if (g_IsUsingARM)
    {
        vARMDesc = g_ARMTexture.Sample(g_LinearSampler, In.vTexcoord);
        fDepthARM = In.vProjPos.z / In.vProjPos.w;

    }

    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, fDepthARM, 0.f);
    Out.vARM = vARMDesc;
    
    return Out;

}

float4 PS_CASCADE_ARRAY(GS_OUT In) : SV_Target0
{
    return float4(In.vPosition.z, 0.0f, 0.0f, 0.0f);
    
}

technique11 DefaultTechnique
{
    pass Default // 0
    {
        SetRasterizerState(RS_Default);
        //SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Cascade // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_CASCADE();
        GeometryShader = compile gs_5_0 GS_SHADOW();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_CASCADE_ARRAY();
    }

    pass NonCulling // 2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}