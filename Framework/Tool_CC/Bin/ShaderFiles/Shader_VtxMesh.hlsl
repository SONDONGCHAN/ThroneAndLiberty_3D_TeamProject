#include "Shader_Defines.hlsli"
#include "Shader_Function.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vCamPos;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_ARMTexture;
Texture2D g_EmissionTexture;
Texture2D g_AOTexture;
Texture2D g_CloudMaskTexture;

float4x4 g_CascadeProjMat[3];
float4x4 g_CascadeViewMat[3];
float4x4 g_CascadeWorldMat[3];

int g_iCascadeNum; 

bool g_IsUsingNormal, g_IsUsingARM, g_IsUsingEmission, g_IsUsingAO;
bool g_IsUsingRim = false;

//For. Pixel Picking
uint g_ObjectID = 0;

bool g_bUseGlowColor = false;
float4 g_vGlowColor = { 1.f, 1.f, 1.f, 1.f };
float g_fAlpha;

float3 g_vRimColor;
float g_fRimPow = 1.5f;

float g_fScale = 1.f;
float g_fLineThick = 1.f;

// Dissolve
bool g_bDslv = false;
float g_fDslvValue = 0.f;
float g_fDslvThick = 1.f;
float4 g_vDslvColor;
Texture2D g_DslvTexture;

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


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
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

struct VS_CASCADE_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_CASCADE_OUT VS_CASCADE(VS_IN In)
{
    VS_CASCADE_OUT Out = (VS_CASCADE_OUT) 0;
    Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;

}


struct GS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
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
           Output.vTexcoord = Input[v].vTexcoord;
            DataStream.Append(Output);
        }
    
        DataStream.RestartStrip();
    
    }

    
}


// JJH For.Outline Depth
struct VS_BASIC_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
};

VS_BASIC_OUT VS_BASIC(VS_IN In)
{
    VS_BASIC_OUT Out = (VS_BASIC_OUT)0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));

    return Out;
}

VS_BASIC_OUT VS_OVERSIZE(VS_IN In)
{
    VS_BASIC_OUT Out = (VS_BASIC_OUT)0;

    matrix matWV, matVP, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    vector vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix) + (vNormal * g_fLineThick * 0.02f);

    Out.vPosition = mul(vPosition, matVP);
    Out.vNormal = vNormal;

    return Out;
}

VS_OUT VS_SIZEUP(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    matrix matWV, matVP, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vProjPos = mul(vector(In.vPosition, 1.f), matWVP);

    vector vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix) + (vNormal * (g_fScale * 0.05f));

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
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vARM : SV_TARGET3;
    vector vRim : SV_TARGET4;
};

struct PS_GLOW_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    float3 vNormal;
    
    if (g_bDslv)
    {
        vector vDslvTexture = g_DslvTexture.Sample(g_LinearSampler, In.vTexcoord);
        vector vDslvDiffuse = Dissolve(vDslvTexture.r, g_fDslvValue - 0.1f, g_fDslvValue, g_fDslvThick * 20.f, g_vDslvColor, vMtrlDiffuse);

        /*if (vDslvTexture.r >= g_fDslvValue)
        {
            vMtrlDiffuse = vector(1.f, 1.f, 1.f, 1.f);
            vMtrlDiffuse.a = 0.f;
            discard;
        }
        else
            vMtrlDiffuse = vector(g_fDslvValue, 0.f, 0.f, 1.f);*/


        if (vDslvDiffuse.a <= 0.f)
            discard;

        vMtrlDiffuse = vDslvDiffuse;
    }
    
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
    
    Out.vRim = float4(0.f, 0.f, 0.f ,0.f);
    float4 TestRim = float4(0.f, 0.f, 0.f, 0.f);
    
    /*림라이트 사용*/
    if (g_IsUsingRim)
    {
        float3 vRimFinalColor = ComputeRimColor(g_vRimColor, g_vCamPos.xyz - g_WorldMatrix[3].xyz, vNormal.xyz, g_fRimPow);
       // Out.vRim = float4(vRimFinalColor, 1.f);
        TestRim = float4(vRimFinalColor, 1.f);
        
    }
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vRim = TestRim;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, fDepthARM, 0.f);
    //   Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    Out.vARM = vARMDesc;
    
    return Out;
}


struct PS_BASIC_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
};

struct PS_BASIC_OUT
{
    vector vPosition : SV_TARGET0;
};

float4 PS_OUTLINE(PS_BASIC_IN In) : SV_TARGET0
{
    return float4(In.vPosition.z, 0.0f, 0.0f, 0.0f);
}

PS_BASIC_OUT PS_OVERSIZE(PS_BASIC_IN In)
{
    PS_BASIC_OUT Out = (PS_BASIC_OUT)0;

    vector vPosition = vector(In.vPosition);

    Out.vPosition = float4(1.f, 1.f, 1.f, 1.f);

    return Out;
}

float4 PS_CASCADE(VS_CASCADE_OUT In) : SV_Target0
{   
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    return float4(In.vPosition.z, 0.f, 0.f, 0.f);
}

PS_OUT PS_RIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

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

    Out.vRim = float4(0.f, 0.f, 0.f, 0.f);
    float4 TestRim = float4(0.f, 0.f, 0.f, 0.f);

    /*림라이트 사용*/
    if (g_IsUsingRim)
    {
        float3 vRimFinalColor = ComputeRimColor(g_vRimColor, g_vCamPos.xyz - g_WorldMatrix[3].xyz, vNormal.xyz, g_fRimPow);
        // Out.vRim = float4(vRimFinalColor, 1.f);
        TestRim = float4(vRimFinalColor, 1.f);

    }

    Out.vDiffuse = vector(0.f, 0.f, 0.f, 0.f);
    Out.vRim = TestRim;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, fDepthARM, 0.f);
    //   Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    Out.vARM = vARMDesc;

    return Out;
}

PS_GLOW_OUT PS_GLOW(PS_IN In)
{
    PS_GLOW_OUT Out = (PS_GLOW_OUT)0;

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

float4 PS_TRAIL(PS_IN In) : SV_Target0
{
    float3 vRimFinalColor = ComputeRimColor(g_vRimColor, g_vCamPos.xyz - g_WorldMatrix[3].xyz, In.vNormal.xyz, g_fRimPow);
    return float4(vRimFinalColor, 1.f);
}

//For. Pixel Picking
struct PS_PIXELPICKING_OUT
{
    vector vPixelPicking : SV_TARGET0;
    vector vPixelPickingView : SV_TARGET1;
};
  
 //For. Pixel Picking
PS_PIXELPICKING_OUT PS_PIXELPICKING(PS_IN In)
{
    PS_PIXELPICKING_OUT Out = (PS_PIXELPICKING_OUT) 0;

   
    Out.vPixelPicking = float4(g_WorldMatrix[3].xyz, g_ObjectID);
    Out.vPixelPickingView = float4(In.vWorldPos.x / 513.f, (In.vWorldPos.y / 513.f) * 50.f, In.vWorldPos.z / 513.f, 1.f);
    
    if (Out.vPixelPicking.a <= 0.8f)
        discard;
    
    return Out;
}

PS_OUT PS_SKY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vCloud = g_CloudMaskTexture.Sample(g_LinearSampler, In.vTexcoord);
    Out.vDiffuse = vCloud;
    return Out;
}


technique11 DefaultTechnique
{
    pass Default // 0
    {
        //SetRasterizerState(RS_WireFrame);
        SetRasterizerState(RS_Default);
        //SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Cascade //1 
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Greater, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_CASCADE();
        GeometryShader = compile gs_5_0 GS_SHADOW();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_CASCADE();
    }

    pass OutlineDepth // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_BASIC();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE();
    }

    pass RimLight // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_RIM();
    }

    pass Glow // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_GLOW();
    }


    pass Trail // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL();
    }

    pass OverSizeNoneZ //6
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_OVERSIZE();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_OVERSIZE();
    }

    pass OverSize //7
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_OVERSIZE();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_OVERSIZE();
    }

    pass Sky //8
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_SKY();
    }
    
    //For. Pixel Picking
    pass PixelPicking //9
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_PIXELPICKING();
    }

    pass NonCulling //10
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