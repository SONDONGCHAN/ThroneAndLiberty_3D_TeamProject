
#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_RotationMatrix;
vector g_vCamPosition;

float g_fRatio;
float g_fDiscardAlpha = 0.f;

float2 g_vTexcoordTL;
float2 g_vTexcoordBR;

unsigned int g_iTexType;
unsigned int g_iRenderState;

float2 g_vMousePos;
float g_fRadius;

float g_fVisiblePosY;
float g_fVisibleSizeY;

/* Slot(Item, Skill) */
unsigned int g_iSlotType;
unsigned int g_iSlotUV; // 0일 경우 Top이 걸친거고, 1일 경우 Bottom이 걸친것이다
float g_fExpandUV;

float g_fCoolTimeRatio;
float2 g_vPole = { 0.5f, 0.5f };

float g_fAlpha = 1.f;

unsigned int g_iHF;

/* Instance Render State */
/* 0 - Diffuse, 1 - Mask, 2 - Noise, 3 - None */
unsigned int g_iInstanceRenderState = 0;

Texture2D g_DiffuseTexture;
Texture2D g_MaskTexture;
Texture2D g_NoiseTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;

    row_major float4x4 TransformMatrix : WORLD;

    float4 vColor : COLOR0;
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
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
	
    //Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vPosition = vPosition;
    Out.vPSize = float2(length(In.TransformMatrix._11_12_13) * In.vPSize.x,
		length(In.TransformMatrix._21_22_23) * In.vPSize.y);
    Out.vColor = In.vColor;

    return Out;
}

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

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

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

[maxvertexcount(6)]
void GS_MAIN_UI(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];
    
    float3 vLook = float3(0.f, 0.f, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    //matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);
    
    //vRight = mul(vRight, g_RotationMatrix);
    //vUp = mul(vUp, g_RotationMatrix);
    
    Out[0].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matWVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vColor = In[0].vColor;
	
    Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matWVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vColor = In[0].vColor;
    
    Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matWVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vColor = In[0].vColor;
    
    Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matWVP);
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

[maxvertexcount(6)]
void GS_MAIN_UI_BAR(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];
    
    float3 vLook = float3(0.f, 0.f, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = In[0].vPosition + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vColor = In[0].vColor;
	
    Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vColor = In[0].vColor;
    
    Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vColor = In[0].vColor;
    
    Out[3].vPosition = In[0].vPosition - float4(vUp, 0.f);
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

[maxvertexcount(6)]
void GS_MAIN_UI_UV(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];
    
    float3 vLook = float3(0.f, 0.f, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(g_vTexcoordTL.x, g_vTexcoordTL.y);
    Out[0].vColor = In[0].vColor;
	
    Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(g_vTexcoordBR.x, g_vTexcoordTL.y);
    Out[1].vColor = In[0].vColor;
    
    Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(g_vTexcoordBR.x, g_vTexcoordBR.y);
    Out[2].vColor = In[0].vColor;
    
    Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(g_vTexcoordTL.x, g_vTexcoordBR.y);
    Out[3].vColor = In[0].vColor;
    
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}

[maxvertexcount(6)] /* 확대 */
void GS_MAIN_UI_EXP(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];
    
    float3 vLook = float3(0.f, 0.f, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    //matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);
    
    //vRight = mul(vRight, g_RotationMatrix);
    //vUp = mul(vUp, g_RotationMatrix);
    
    Out[0].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matWVP);
    Out[0].vTexcoord = float2(g_fExpandUV, g_fExpandUV);
    Out[0].vColor = In[0].vColor;
	
    Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matWVP);
    Out[1].vTexcoord = float2(1.f - g_fExpandUV, g_fExpandUV);
    Out[1].vColor = In[0].vColor;
    
    Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matWVP);
    Out[2].vTexcoord = float2(1.f - g_fExpandUV, 1.f - g_fExpandUV);
    Out[2].vColor = In[0].vColor;
    
    Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matWVP);
    Out[3].vTexcoord = float2(g_fExpandUV, 1.f - g_fExpandUV);
    Out[3].vColor = In[0].vColor;
    
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}

[maxvertexcount(6)] /* Reverse */
void GS_MAIN_UI_RH(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];
    
    float3 vLook = float3(0.f, 0.f, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    //matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);
    
    //vRight = mul(vRight, g_RotationMatrix);
    //vUp = mul(vUp, g_RotationMatrix);
    
    Out[0].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matWVP);
    Out[0].vTexcoord = float2(1.f, 0.f);
    Out[0].vColor = In[0].vColor;
	
    Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matWVP);
    Out[1].vTexcoord = float2(0.f, 0.f);
    Out[1].vColor = In[0].vColor;
    
    Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matWVP);
    Out[2].vTexcoord = float2(0.f, 1.f);
    Out[2].vColor = In[0].vColor;
    
    Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matWVP);
    Out[3].vTexcoord = float2(1.f, 1.f);
    Out[3].vColor = In[0].vColor;
    
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}

[maxvertexcount(6)] /* Reverse */
void GS_MAIN_UI_RV(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];
    
    float3 vLook = float3(0.f, 0.f, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    //matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);
    
    //vRight = mul(vRight, g_RotationMatrix);
    //vUp = mul(vUp, g_RotationMatrix);
    
    Out[0].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matWVP);
    Out[0].vTexcoord = float2(0.f, 1.f);
    Out[0].vColor = In[0].vColor;
	
    Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matWVP);
    Out[1].vTexcoord = float2(1.f, 1.f);
    Out[1].vColor = In[0].vColor;
    
    Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matWVP);
    Out[2].vTexcoord = float2(1.f, 0.f);
    Out[2].vColor = In[0].vColor;
    
    Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matWVP);
    Out[3].vTexcoord = float2(0.f, 0.f);
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
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    if (g_iTexType == 0)
    {
        Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    }
    
    if (g_iTexType == 1)
    {
        vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
        vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    
        Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;

    }
    
    if (g_iTexType == 2)
    {
        vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord);
    
        Out.vColor = vMaskMtrl.r * In.vColor;
    }
    
    if (g_iTexType == 3)
    {
        Out.vColor = In.vColor;
    }
    
    if (g_iTexType == 8)
        discard;
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_SLIDER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    if (In.vTexcoord.x > g_vMousePos.x - g_fRadius && In.vTexcoord.x < g_vMousePos.x + g_fRadius &&
        In.vTexcoord.y > g_vMousePos.y - g_fRadius && In.vTexcoord.y < g_vMousePos.y + g_fRadius)
    {
        if (g_iTexType == 0)
        {
            Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        }
    
        if (g_iTexType == 1)
        {
            vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
            vector vMaskMtrl = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        
            Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;
        }
    
        if (g_iTexType == 2)
        {
            vector vMaskMtrl = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
        
            Out.vColor = vMaskMtrl * In.vColor;
        }
    
        if (g_iTexType == 3)
        {
            Out.vColor = In.vColor;
        }
        
        if (g_iTexType == 8)
            discard;
    }
    
    else
    {
        Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    }
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_BAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    if (In.vTexcoord.x <= g_fRatio)
    {
        if (g_iTexType == 0)
        {
            Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        }
    
        if (g_iTexType == 1)
        {
            vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
            vector vMaskMtrl = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        
            Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;
        }
    
        if (g_iTexType == 2)
        {
            vector vMaskMtrl = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
        
            Out.vColor = vMaskMtrl * In.vColor;
        }
    
        if (g_iTexType == 3)
        {
            Out.vColor = In.vColor;
        }
        
        if (g_iTexType == 8)
            discard;
    }
    else
        discard;
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_SCROLL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
   
    if (In.vTexcoord.y >= (g_fVisiblePosY - (g_fVisibleSizeY / 2.f)) && In.vTexcoord.y <= (g_fVisiblePosY + (g_fVisibleSizeY / 2.f)))
    {
        Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    }
    else
        discard;
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_SLOT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (g_iSlotType == 1)
    {
        if (g_iSlotUV == 0) // Top이 걸친 상황
        {
            if (In.vTexcoord.y <= g_fRatio)
                Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        }
        else if (g_iSlotUV == 1) // Bottom이 걸친 상황
        {
            if (In.vTexcoord.y >= g_fRatio)
                Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        }
        else if (g_iSlotUV == 2)
            Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        else if (g_iSlotUV == 3)
            discard;
        else
            Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
    }
    
    else if (g_iSlotType == 5)
    {
        if (g_iTexType == 0)
        {
            Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        }
    
        if (g_iTexType == 1)
        {
            vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
            vector vMaskMtrl = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
    
            Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;

        }
    
        if (g_iTexType == 2)
        {
            vector vMaskMtrl = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
    
            Out.vColor = vMaskMtrl * In.vColor;
        }
    
        if (g_iTexType == 3)
        {
            Out.vColor = In.vColor;
        }
        
        if (g_iTexType == 8)
            discard;
    }
    
    else
    {
        if (g_iTexType == 0)
        {
            Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        }
    
        if (g_iTexType == 1)
        {
            vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
            vector vMaskMtrl = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
    
            Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;
        }
    
        if (g_iTexType == 2)
        {
            vector vMaskMtrl = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
    
            Out.vColor = vMaskMtrl * In.vColor;
        }
    
        if (g_iTexType == 3)
        {
            Out.vColor = In.vColor;
        }
        
        if (g_iTexType == 8)
            discard;
    }
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_COOLTIME(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    //In.vTexcoord.x = (In.vTexcoord.x + 1) / 2.f;
    //In.vTexcoord.y = (1 - In.vTexcoord.y) / 2.f;

    float standardRatio = (radians(180.f) * (2.f * g_fCoolTimeRatio)) - radians(180.f);
    
    //float2x2 rotationMat = float2x2(cos(radians(90.f)), -1.f * sin(radians(90.f)), sin(radians(90.f)), cos(radians(90.f)));
    float2x2 rotationMat = float2x2(0.f, -1.f, 1.f, 0.f);
    
    //float2 vPole = float2(1.f, 0.5f) - float2(0.5f, 0.5f);
    //vPole = normalize(vPole);
    
    float2 vPole = float2(1.f, 0.f);
    vPole = mul(vPole, rotationMat);
    float2 vUV = In.vTexcoord - g_vPole;
    vUV = normalize(vUV);
    float2 atanVec = vUV - vPole;
    float atan2Ret = atan2(atanVec.y, atanVec.x);
    
    if (atan2Ret >= standardRatio)
        Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
    else
    {
        //Out.vColor = float4(1.f, 0.f, 0.f, 1.f) * In.vColor;
        
        discard;
    }
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_PANEL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    if (g_iRenderState == 0)
    {
        if (g_iTexType == 0)
        {
            Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
        }
    
        if (g_iTexType == 1)
        {
            vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
            vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    
            Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;

        }
    
        if (g_iTexType == 2)
        {
            vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord);
    
            Out.vColor = vMaskMtrl * In.vColor;
        }
    
        if (g_iTexType == 3)
        {
            Out.vColor = In.vColor;
        }
    }
    
    else if (g_iRenderState == 1)
        Out.vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_ITEMBG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (g_iSlotUV == 0) // Top이 걸친 상황
    {
        if (In.vTexcoord.y <= g_fRatio)
            Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    }
    else if (g_iSlotUV == 1) // Bottom이 걸친 상황
    {
        if (In.vTexcoord.y >= g_fRatio)
            Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    }
    else if (g_iSlotUV == 2)
        Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    else if (g_iSlotUV == 3)
        discard;
    else
        Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_HF(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    if (g_iHF == 0)
    {
        if (In.vTexcoord.x > 0.5f)
            discard;
    }
    
    if (g_iHF == 1)
    {
        if (In.vTexcoord.x < 0.5f)
            discard;
    }
    
    if (g_iTexType == 0)
    {
        Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    }
    
    if (g_iTexType == 1)
    {
        vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
        vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    
        Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;

    }
    
    if (g_iTexType == 2)
    {
        vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord);
    
        Out.vColor = vMaskMtrl * In.vColor;
    }
    
    if (g_iTexType == 3)
    {
        Out.vColor = In.vColor;
    }
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_WORLD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    if (g_iTexType == 0)
    {
        Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    }
    
    if (g_iTexType == 1)
    {
        vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
        vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    
        Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;

    }
    
    if (g_iTexType == 2)
    {
        vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord);
    
        Out.vColor = vMaskMtrl * In.vColor;
    }
    
    if (g_iTexType == 3)
    {
        Out.vColor = In.vColor;
    }
    
    if (g_iTexType == 8)
        discard;
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_WORLDBAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    if (In.vTexcoord.x <= g_fRatio)
    {
        if (g_iTexType == 0)
        {
            Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
        }
    
        if (g_iTexType == 1)
        {
            vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
            vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
        
            Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;
        }
    
        if (g_iTexType == 2)
        {
            vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord);
        
            Out.vColor = vMaskMtrl * In.vColor;
        }
    
        if (g_iTexType == 3)
        {
            Out.vColor = In.vColor;
        }
        
        if (g_iTexType == 8)
            discard;
    }
    else
        discard;
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_INSTANCEBG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    vector vDiffuseMtrl = float4(1.f, 1.f, 1.f, 1.f);
    vector vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    vector vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
    {
        //Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        vDiffuseMtrl = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    }
    
    if (g_iInstanceRenderState == 1)
    {
        //vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        vMaskMtrl = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
    }
    
    if (g_iInstanceRenderState == 2)
    {
        vNoiseMtrl = g_NoiseTexture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
    }
    
    if (g_iInstanceRenderState == 3)
    {
        Out.vColor = In.vColor;
        
        return Out;
    }
    
    if (g_iInstanceRenderState == 4)
        discard;
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
    
    Out.vColor = ((vDiffuseMtrl * In.vColor) * vMaskMtrl.r) + vNoiseMtrl;
    
    return Out;
}


PS_OUT PS_MAIN_ALPAH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    if (g_iTexType == 0)
    {
        Out.vColor = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    }
    
    if (g_iTexType == 1)
    {
        vector vDiffuseMtrl = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
        vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord) * In.vColor;
    
        Out.vColor = (vDiffuseMtrl * vMaskMtrl.r) * In.vColor;

    }
    
    if (g_iTexType == 2)
    {
        vector vMaskMtrl = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord);
    
        Out.vColor = vMaskMtrl.r * In.vColor;
    }
    
    if (g_iTexType == 3)
    {
        Out.vColor = In.vColor;
    }
    
    if (g_iTexType == 8)
        discard;
    
    if (Out.vColor.a < g_fDiscardAlpha)
        discard;
	
    Out.vColor.a = g_fAlpha;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass UI_Tex // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    
    pass UI_Slider
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SLIDER();
    }

    pass UI_Bar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BAR();
    }

    pass UI_Scroll
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SCROLL();
    }

    pass UI_Slot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SLOT();
    }

    pass UI_CoolTime
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COOLTIME();
    }

    pass UI_UV // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI_UV();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_Panel
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PANEL();
    }

    pass UI_Slot_Abnormal // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI_EXP();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SLOT();
    }

    pass UI_BackGround_RH // 9, Reverse LeftRight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI_RH();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_ItemBackGround // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ITEMBG();
    }

    pass UI_BackGround // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_RV // 12, Reverse UpsideDown
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI_RV();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_HF // 13, Half
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HF();
    }

    pass UI_WorldBG // 14, WorldBG
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WORLD();
    }

    pass UI_WorldBar // 15, WorldBG
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WORLDBAR();
    }

    pass UI_WorldBackGround // 16, WorldBGNT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WORLD();
    }

    pass UI_InstanceBG // 17, InstanceBG
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEBG();
    }


    pass UI_AlphaBG // 18, AlphaBG
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPAH();
    }



//PS_MAIN_ALPAH

}
