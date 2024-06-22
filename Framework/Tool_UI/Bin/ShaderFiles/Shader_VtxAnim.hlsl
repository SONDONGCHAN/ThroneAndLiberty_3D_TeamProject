#include "Shader_Defines.hlsli"
#include "Shader_Function.hlsli"

matrix g_BoneMatrices[512];

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_ARMTexture;
texture2D g_NormalTexture;
texture2D g_EmissionTexture;
texture2D g_AOTexture;

bool g_IsUsingNormal, g_IsUsingARM, g_IsUsingEmission, g_IsUsingAO;

texture2D	g_DistortionTexture;
float		g_fDistortionTimer, g_fDistortionSpeed, g_fDistortionWeight;

bool g_IsUsingRim;
float3 g_vRimColor;
float g_fRimPow = 1.5f;

float4 g_vCamPos;

float g_fFar= 1000.f;

/*플레이어 머리 색 */
float3 g_vHairMainColor, g_vHairSubColor;

/*Bind해서 받는 Input값*/
StructuredBuffer<float> g_Input;
/*GPU에서 연산한 결과를 담은 후 CPU에서 복사해서 사용할 Buffer*/
RWStructuredBuffer<float> g_Output;

uint g_iCascadeNum = 0;

float4x4 g_CascadeProjMat[3];
float4x4 g_CascadeViewMat[3];
float4x4 g_CascadeReverseProjMat[3];

matrix g_LightViewMatrix;
matrix g_LightProjMatrix;

//for. Pixel Picking
//uint g_ObjectID = 0;

float   g_fLineThick = 1.f;
float4  g_vLineColor = { 1.f, 1.f, 1.f, 1.f };

// Dissolve
bool    g_bDslv = false;
float   g_fDslvValue = 0.f;
float   g_fDslvThick = 1.f;
float4  g_vDslvColor;
Texture2D   g_DslvTexture;

// Outline
bool    g_bOutline = false;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
    
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
    float4 vOutline : TEXCOORD3;
};

struct VS_DEPTH_OUT
{
    float4 vDepth : TEXCOORD0;
};

// JJH For.Outline Depth
struct VS_NORMAL_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
};

struct VS_TEXCOORD_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;

    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
    g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
    g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
    g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

    vNormal = normalize(mul(vNormal, g_WorldMatrix));

    /*if (g_bOutline)
    {*/
        vector vOutline = mul(vPosition, g_WorldMatrix) + (vNormal * 0.02f);
        Out.vOutline = mul(vOutline, matVP);
    //}
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = vNormal;
    Out.vProjPos = Out.vPosition;
    
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

    
    return Out;

}

VS_OUT VS_RESIZE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
    g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
    g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
    g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    vNormal = normalize(mul(vNormal, g_WorldMatrix));
    vPosition = mul(vPosition, g_WorldMatrix) + (vNormal * 0.01f);
    
    Out.vPosition = mul(vPosition, matVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = vNormal;
    Out.vProjPos = Out.vPosition;
    
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

    
    return Out;

}

VS_NORMAL_OUT VS_NORMAL(VS_IN In)
{
    VS_NORMAL_OUT Out = (VS_NORMAL_OUT)0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
        g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
        g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
        g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));

    return Out;
}

VS_TEXCOORD_OUT VS_TEXCOORD(VS_IN In)
{
    VS_TEXCOORD_OUT Out = (VS_TEXCOORD_OUT)0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
        g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
        g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
        g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/*Cascade Shadow*/
struct VS_SHADOW_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_NORMAL_OUT VS_OVERSIZE(VS_IN In)
{
    VS_NORMAL_OUT Out = (VS_NORMAL_OUT)0;

    matrix matVP;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    matVP = mul(g_ViewMatrix, g_ProjMatrix);

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
        g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
        g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
        g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

    vNormal = normalize(mul(vNormal, g_WorldMatrix));
    vPosition = mul(vPosition, g_WorldMatrix) + (vNormal * g_fLineThick * 0.02f);

    Out.vPosition = mul(vPosition, matVP);
    Out.vNormal = vNormal;
    
    return Out;
}


VS_OUT VS_SHADOW(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_LightViewMatrix);
    matWVP = mul(matWV, g_LightProjMatrix);
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
    g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
    g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
    g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition;

    return Out;

}

struct GS_IN
{
    float4 vPosition : SV_POSITION;

};



VS_OUT VS_CASCADE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_CascadeViewMat[g_iCascadeNum]);
    matWVP = mul(matWV, g_CascadeProjMat[g_iCascadeNum]);
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
    g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
    g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
    g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    
    /*Cascade에서 구한 View + Projection 행렬을 사용할 거기 때문에 로컬 자체를 넘긴다*/
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition;
    return Out;

}

GS_IN VS_CASCADE_ARRAY(VS_IN In)
{
    GS_IN Out = (GS_IN) 0;
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
    g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
    g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
    g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    return Out;
}


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

float4 PS_CASCADE_ARRAY(GS_OUT In) : SV_Target0
{   
    return float4(In.vPosition.z, 0.0f, 0.0f, 0.0f);
    
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
    float4 vOutline : TEXCOORD3;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vARM : SV_TARGET3;
    vector vRim : SV_TARGET4;
    vector vPixelPicking : SV_TARGET5;
    vector vOutline : SV_TARGET6;

};

struct PS_NORMAL_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
};

struct PS_TEXCOORD_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_POSITION_OUT
{
    vector vPosition : SV_TARGET0;
};

struct PS_COLOR_OUT
{
    vector vColor : SV_TARGET0;
};

struct PS_CASCADE_OUT
{
    vector vColor1 : SV_TARGET0;
    vector vColor2 : SV_TARGET1;
    vector vColor3 : SV_TARGET2;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;

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
    
    float3 vNormal;

        
    if (g_IsUsingNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(g_LinearSampler, In.vTexcoord);

        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        vNormal = mul(vNormal, WorldMatrix);
    }
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
        /*모든 오브젝트가 ARM 텍스쳐를 가지고 있지 않는데 더하기 연산을 수행해서 투과되는 현상이 있음*/
        /*Depth z성분에 월드 포지션 z를 저장해주고 라이팅 연산에서 비교해서 다르면 연산하지 않는다*/
        vARMDesc = g_ARMTexture.Sample(g_LinearSampler, In.vTexcoord);
        fDepthARM = In.vProjPos.z / In.vProjPos.w;

    }
     
    Out.vRim = float4(0.f, 0.f, 0.f, 0.f);
    float4 TestRim = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_IsUsingRim)
    {
        float3 vRimFinalColor = ComputeRimColor(g_vRimColor, g_vCamPos.xyz - g_WorldMatrix[3].xyz, vNormal.xyz, g_fRimPow);
        Out.vRim = float4(vRimFinalColor, 0.8f);
    }

    vector vOutline = vector(In.vOutline);

    //Out.vOutline = float4(0.f, 0.f, 0.f, 1.f);

    if (g_bOutline)
    {
        Out.vOutline = vOutline;
    }
    
    Out.vDiffuse = float4(vMtrlDiffuse.xyz, 1);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, fDepthARM, 0.f);
    Out.vARM = vARMDesc;
    
    return Out;
}



PS_OUT PS_MAIN_NONALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

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
    
    float3 vNormal;

        
    if (g_IsUsingNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(g_LinearSampler, In.vTexcoord);

        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        vNormal = mul(vNormal, WorldMatrix);
    }
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
        /*모든 오브젝트가 ARM 텍스쳐를 가지고 있지 않는데 더하기 연산을 수행해서 투과되는 현상이 있음*/
        /*Depth z성분에 월드 포지션 z를 저장해주고 라이팅 연산에서 비교해서 다르면 연산하지 않는다*/
        vARMDesc = g_ARMTexture.Sample(g_LinearSampler, In.vTexcoord);
        fDepthARM = In.vProjPos.z / In.vProjPos.w;

    }
     
    Out.vRim = float4(0.f, 0.f, 0.f, 0.f);
    float4 TestRim = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_IsUsingRim)
    {
        float3 vRimFinalColor = ComputeRimColor(g_vRimColor, g_vCamPos.xyz - g_WorldMatrix[3].xyz, vNormal.xyz, g_fRimPow);
        Out.vRim = float4(vRimFinalColor, 0.8f);
    }

    vector vOutline = vector(In.vOutline);

    //Out.vOutline = float4(0.f, 0.f, 0.f, 1.f);

    if (g_bOutline)
    {
        Out.vOutline = vOutline;
    }
    
    Out.vDiffuse = float4(vMtrlDiffuse.xyz, 1);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, fDepthARM, 0.f);
    Out.vARM = vARMDesc;
    
    return Out;
}

PS_OUT PS_HAIR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;    
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
   // float fColorRate = (vMtrlDiffuse.g + vMtrlDiffuse.r) / 2.f;
    
    //float3 vFinHairColor = lerp(g_vHairSubColor, g_vHairMainColor, fColorRate);
    
    float3 haircolor_1 = g_vHairMainColor * vMtrlDiffuse.g;
    float3 haircolor_2 = g_vHairSubColor * (1.f - vMtrlDiffuse.g);
		
    float3 vBlendedHairColor = haircolor_1 + haircolor_2;
    Out.vDiffuse = saturate(float4(vBlendedHairColor.rgb, 1.f));
        
    Out.vDiffuse = float4(vBlendedHairColor.xyz, 1);
   
         
    Out.vRim = float4(0.f, 0.f, 0.f, 0.f);
    float4 TestRim = float4(0.f, 0.f, 0.f,
    0.f);

    float3 vNormal = In.vNormal.xyz * 2.f - 1.f;

    
    if (g_IsUsingRim)
    {
        float3 vRimFinalColor = ComputeRimColor(g_vRimColor, g_vCamPos.xyz - g_WorldMatrix[3].xyz, In.vNormal.xyz, g_fRimPow);
        //TestRim = float4(vRimFinalColor, 1.f);
        //Out.vRim = float4(vRimFinalColor, 0.2f);
    }
    
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar , 0.f, 0.f);
    Out.vARM = float4(1.f, 0.f, 0.f, 0.f);
    
    return Out;
}


float4 PS_TRAIL(PS_IN In) :SV_Target0
{
 //   float3 vNormal = In.vNormal.xyz * 2.f - 1.f; 
    float3 vRimFinalColor = ComputeRimColor(g_vRimColor, g_vCamPos.xyz - g_WorldMatrix[3].xyz, In.vNormal.xyz, g_fRimPow);
    return float4(vRimFinalColor, 1.f);
}


float4 PS_SHADOW(GS_OUT In) : SV_Target0
{
    return float4(1.f, 0.f, 0.f, 1.f);

}

float4 PS_CASCADE_SHADOW(PS_IN In) : SV_Target0
{    
    return float4(In.vPosition.z, 0.0f, 0.0f, 0.0f);
}

PS_CASCADE_OUT PS_CASCADE_SHADOW2(PS_IN In) : SV_Target0
{
    PS_CASCADE_OUT Out = (PS_CASCADE_OUT)0;
    
    if (In.vPosition.z <= 20.f)
    {
        Out.vColor1 = float4(In.vPosition.z, 0.0f, 0.0f, 0.0f);

    }
    
    
    return Out;
}

//float4 PS_OUTLINE(PS_NORMAL_IN In) : SV_TARGET0
//{
//    return float4(In.vPosition.z, 0.0f, In.vSizeUpPosition.z, 0.0f);
//}

//float4 PS_OUTLINE(PS_NORMAL_IN In) : SV_TARGET0
//{
//    return float4(In.vPosition.z, 0.0f, 0.0f, 0.0f);
//}

//void PS_OUTLINE(PS_NORMAL_IN In, out float4 Depth : SV_TARGET0, out float4 Outline : SV_TARGET1)
//{
//    Depth = In.vPosition;
//    Outline = In.vSizeUpPosition;
//
//    return;
//}

float4 PS_OUTLINE(PS_NORMAL_IN In) : SV_TARGET0
{
    return float4(In.vPosition.z, 0.0f, 0.0f, 0.0f);
}

vector PS_MAIN_SHADOW(PS_IN In) : SV_TARGET0
{
    vector vLightViewDepth = (vector) 0.f;

    vLightViewDepth = vector(In.vProjPos.w / 0.1f, 0.f, 0.f, 1.f);

    return vLightViewDepth;
}
 
PS_POSITION_OUT PS_OVERSIZE(PS_NORMAL_IN In)
{
    PS_POSITION_OUT Out = (PS_POSITION_OUT)0;

    vector vPosition = vector(In.vPosition);

    Out.vPosition = g_vLineColor;

    return Out;
}

PS_COLOR_OUT PS_DISTORTION(PS_TEXCOORD_IN In)
{
    PS_COLOR_OUT Out = (PS_COLOR_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

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

    vector	vDistortionTexture = g_DistortionTexture.Sample(g_LinearSampler, In.vTexcoord + g_fDistortionTimer * g_fDistortionSpeed);

    Out.vColor = vDistortionTexture;
    Out.vColor.a = g_fDistortionWeight;

    return Out;
}


//float4 PS_OVERSIZE(PS_NORMAL_IN In) : SV_TARGET0
//{
//    return In.vPosition;
//}

//vector PS_OVERSIZE(PS_NORMAL_IN In) : SV_TARGET0
//{
//    vector vPosition = vector(In.vPosition);
//
//    return vPosition;
//}

technique11 DefaultTechnique
{
    pass Default //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Hair //1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_HAIR();
    }

    pass Resize //2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass CascadeShadow //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Greater, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_CASCADE_ARRAY();
        GeometryShader = compile gs_5_0 GS_SHADOW();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_CASCADE_ARRAY();
    }


    pass Shadow // 4 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Greater, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SHADOW();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass OutlineDepth //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_NORMAL();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE();
    }

    pass Trail //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL();
    }

    pass OverSizeNoneZ //7
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_OVERSIZE();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_OVERSIZE();
    }

    pass OverSize //8
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_OVERSIZE();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_OVERSIZE();
    }

    pass Distortion //9
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_TEXCOORD();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass NonAlpha //10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONALPHA();
    }

}