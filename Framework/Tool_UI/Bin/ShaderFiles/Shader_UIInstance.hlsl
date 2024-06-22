
#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_RotationMatrix;
vector g_vCamPosition;

float g_fRatio;
float g_fDiscardAlpha = 0.f;

float2 g_vTexcoordTL;
float2 g_vTexcoordBR;

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

unsigned int g_iHF;

/* Instance Render State */
/* 0 - Diffuse, 1 - Mask, 2 - Noise, 3 - None */
unsigned int g_iInstanceRenderState = 0;

float4 g_vColor = float4(1.f, 1.f, 1.f, 1.f);

Texture2D g_DiffuseTexture0;
Texture2D g_DiffuseTexture1;
Texture2D g_DiffuseTexture2;
Texture2D g_DiffuseTexture3;
Texture2D g_DiffuseTexture4;
Texture2D g_DiffuseTexture5;
Texture2D g_DiffuseTexture6;
Texture2D g_DiffuseTexture7;
Texture2D g_DiffuseTexture8;
Texture2D g_DiffuseTexture9;
Texture2D g_DiffuseTexture10;
Texture2D g_DiffuseTexture11;
Texture2D g_DiffuseTexture12;
Texture2D g_DiffuseTexture13;
Texture2D g_DiffuseTexture14;
Texture2D g_DiffuseTexture15;
Texture2D g_DiffuseTexture16;
Texture2D g_DiffuseTexture17;
Texture2D g_DiffuseTexture18;
Texture2D g_DiffuseTexture19;
Texture2D g_DiffuseTexture20;
Texture2D g_DiffuseTexture21;
Texture2D g_DiffuseTexture22;
Texture2D g_DiffuseTexture23;
Texture2D g_DiffuseTexture24;
Texture2D g_DiffuseTexture25;
Texture2D g_DiffuseTexture26;
Texture2D g_DiffuseTexture27;
Texture2D g_DiffuseTexture28;
Texture2D g_DiffuseTexture29;
Texture2D g_DiffuseTexture30;
Texture2D g_DiffuseTexture31;
Texture2D g_DiffuseTexture32;
Texture2D g_DiffuseTexture33;
Texture2D g_DiffuseTexture34;
Texture2D g_DiffuseTexture35;
Texture2D g_DiffuseTexture36;
Texture2D g_DiffuseTexture37;
Texture2D g_DiffuseTexture38;
Texture2D g_DiffuseTexture39;
Texture2D g_DiffuseTexture40;
Texture2D g_DiffuseTexture41;
Texture2D g_DiffuseTexture42;
Texture2D g_DiffuseTexture43;
Texture2D g_DiffuseTexture44;
Texture2D g_DiffuseTexture45;
Texture2D g_DiffuseTexture46;
Texture2D g_DiffuseTexture47;
Texture2D g_DiffuseTexture48;
Texture2D g_DiffuseTexture49;
Texture2D g_DiffuseTexture50;
Texture2D g_MaskTexture;
Texture2D g_NoiseTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;

    row_major float4x4 TransformMatrix : WORLD;

    float4 vColor : COLOR0;
    
    unsigned int iSlotUV : TEXCOORD1;
    float2 vUVRatio : TEXCOORD2;
    unsigned int iTexIndex : TEXCOORD3;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float4 vColor : COLOR0;
    
    unsigned int iSlotUV : TEXCOORD1;
    float2 vUVRatio : TEXCOORD2;
    unsigned int iTexIndex : TEXCOORD3;
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
    Out.iSlotUV = In.iSlotUV;
    Out.vUVRatio = In.vUVRatio;
    Out.iTexIndex = In.iTexIndex;

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float4 vColor : COLOR0;
    
    unsigned int iSlotUV : TEXCOORD1;
    float2 vUVRatio : TEXCOORD2;
    unsigned int iTexIndex : TEXCOORD3;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : COLOR0;
    
    unsigned int iSlotUV : TEXCOORD1;
    float2 vUVRatio : TEXCOORD2;
    unsigned int iTexIndex : TEXCOORD3;
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
    Out[0].iSlotUV = In[0].iSlotUV;
    Out[0].vUVRatio = In[0].vUVRatio;
    Out[0].iTexIndex = In[0].iTexIndex;
	
    Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matWVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vColor = In[0].vColor;
    Out[1].iSlotUV = In[0].iSlotUV;
    Out[1].vUVRatio = In[0].vUVRatio;
    Out[1].iTexIndex = In[0].iTexIndex;
    
    Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matWVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vColor = In[0].vColor;
    Out[2].iSlotUV = In[0].iSlotUV;
    Out[2].vUVRatio = In[0].vUVRatio;
    Out[2].iTexIndex = In[0].iTexIndex;
    
    Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matWVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vColor = In[0].vColor;
    Out[3].iSlotUV = In[0].iSlotUV;
    Out[3].vUVRatio = In[0].vUVRatio;
    Out[3].iTexIndex = In[0].iTexIndex;
    
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

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matWVP);
    Out[0].vTexcoord = float2(g_vTexcoordTL.x, g_vTexcoordTL.y);
    Out[0].vColor = In[0].vColor;
    Out[0].iSlotUV = In[0].iSlotUV;
    Out[0].vUVRatio = In[0].vUVRatio;
    Out[0].iTexIndex = In[0].iTexIndex;
	
    Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matWVP);
    Out[1].vTexcoord = float2(g_vTexcoordBR.x, g_vTexcoordTL.y);
    Out[1].vColor = In[0].vColor;
    Out[1].iSlotUV = In[0].iSlotUV;
    Out[1].vUVRatio = In[0].vUVRatio;
    Out[1].iTexIndex = In[0].iTexIndex;
    
    Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matWVP);
    Out[2].vTexcoord = float2(g_vTexcoordBR.x, g_vTexcoordBR.y);
    Out[2].vColor = In[0].vColor;
    Out[2].iSlotUV = In[0].iSlotUV;
    Out[2].vUVRatio = In[0].vUVRatio;
    Out[2].iTexIndex = In[0].iTexIndex;
    
    Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matWVP);
    Out[3].vTexcoord = float2(g_vTexcoordTL.x, g_vTexcoordBR.y);
    Out[3].vColor = In[0].vColor;
    Out[3].iSlotUV = In[0].iSlotUV;
    Out[3].vUVRatio = In[0].vUVRatio;
    Out[3].iTexIndex = In[0].iTexIndex;
    
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

[maxvertexcount(6)]
void GS_MAIN_QUESTINDICATOR(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    if (In[0].iSlotUV == 1)
    {
        GS_OUT Out[4];
    
        float3 vLook = float3(0.f, 0.f, 1.f);
        float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
        float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

        matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
        matrix matWVP = mul(matWV, g_ProjMatrix);

        Out[0].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
        Out[0].vPosition = mul(Out[0].vPosition, matWVP);
        Out[0].vTexcoord = float2(g_vTexcoordTL.x, g_vTexcoordTL.y);
        Out[0].vColor = In[0].vColor;
        Out[0].iSlotUV = In[0].iSlotUV;
        Out[0].vUVRatio = In[0].vUVRatio;
        Out[0].iTexIndex = In[0].iTexIndex;
	
        Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
        Out[1].vPosition = mul(Out[1].vPosition, matWVP);
        Out[1].vTexcoord = float2(g_vTexcoordBR.x, g_vTexcoordTL.y);
        Out[1].vColor = In[0].vColor;
        Out[1].iSlotUV = In[0].iSlotUV;
        Out[1].vUVRatio = In[0].vUVRatio;
        Out[1].iTexIndex = In[0].iTexIndex;
    
        Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
        Out[2].vPosition = mul(Out[2].vPosition, matWVP);
        Out[2].vTexcoord = float2(g_vTexcoordBR.x, g_vTexcoordBR.y);
        Out[2].vColor = In[0].vColor;
        Out[2].iSlotUV = In[0].iSlotUV;
        Out[2].vUVRatio = In[0].vUVRatio;
        Out[2].iTexIndex = In[0].iTexIndex;
    
        Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
        Out[3].vPosition = mul(Out[3].vPosition, matWVP);
        Out[3].vTexcoord = float2(g_vTexcoordTL.x, g_vTexcoordBR.y);
        Out[3].vColor = In[0].vColor;
        Out[3].iSlotUV = In[0].iSlotUV;
        Out[3].vUVRatio = In[0].vUVRatio;
        Out[3].iTexIndex = In[0].iTexIndex;
    
        DataStream.Append(Out[0]);
        DataStream.Append(Out[1]);
        DataStream.Append(Out[2]);
        DataStream.RestartStrip();

        DataStream.Append(Out[0]);
        DataStream.Append(Out[2]);
        DataStream.Append(Out[3]);
    }
    
    else
    {
        GS_OUT Out[4];
    
        float3 vLook = float3(0.f, 0.f, 1.f);
        float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
        float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;
        
        matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
        matrix matWVP = mul(matWV, g_ProjMatrix);
    
        Out[0].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
        Out[0].vPosition = mul(Out[0].vPosition, matWVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vColor = In[0].vColor;
        Out[0].iSlotUV = In[0].iSlotUV;
        Out[0].vUVRatio = In[0].vUVRatio;
        Out[0].iTexIndex = In[0].iTexIndex;
	
        Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
        Out[1].vPosition = mul(Out[1].vPosition, matWVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vColor = In[0].vColor;
        Out[1].iSlotUV = In[0].iSlotUV;
        Out[1].vUVRatio = In[0].vUVRatio;
        Out[1].iTexIndex = In[0].iTexIndex;
    
        Out[2].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
        Out[2].vPosition = mul(Out[2].vPosition, matWVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vColor = In[0].vColor;
        Out[2].iSlotUV = In[0].iSlotUV;
        Out[2].vUVRatio = In[0].vUVRatio;
        Out[2].iTexIndex = In[0].iTexIndex;
    
        Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
        Out[3].vPosition = mul(Out[3].vPosition, matWVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vColor = In[0].vColor;
        Out[3].iSlotUV = In[0].iSlotUV;
        Out[3].vUVRatio = In[0].vUVRatio;
        Out[3].iTexIndex = In[0].iTexIndex;
    
        DataStream.Append(Out[0]);
        DataStream.Append(Out[1]);
        DataStream.Append(Out[2]);
        DataStream.RestartStrip();

        DataStream.Append(Out[0]);
        DataStream.Append(Out[2]);
        DataStream.Append(Out[3]);
    }
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : COLOR0;
    
    unsigned int iSlotUV : TEXCOORD1;
    float2 vUVRatio : TEXCOORD2; // 첫번째 수가 기존의 slotUV, 두번째 수가 Ratio
    unsigned int iTexIndex : TEXCOORD3; // 무슨 텍스쳐를 사용할 것인지
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_INSTANCEITEMBG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
    {
        unsigned int iSlotUV = In.iSlotUV;
        if (iSlotUV == 0) // Top이 걸친 상황
        {
            if (In.vTexcoord.y <= In.vUVRatio.y)
            {
                //vDiffuseMtrl = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
                if (In.iTexIndex == 0)
                    Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
                else if (In.iTexIndex == 1)
                    Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
            }
            else
                discard;
        }
        else if (iSlotUV == 1) // Bottom이 걸친 상황
        {
            if (In.vTexcoord.y >= In.vUVRatio.y)
            {
                //vDiffuseMtrl = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
                if (In.iTexIndex == 0)
                    Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
                else if (In.iTexIndex == 1)
                    Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
            }
            else
                discard;
        }
        else if (iSlotUV == 2)
        {
            //vDiffuseMtrl = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
            if (In.iTexIndex == 0)
                Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 1)
                Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (iSlotUV == 3)
            discard;
        //else
        //    vDiffuseMtrl = float4(1.f, 1.f, 1.f, 1.f);
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
    
    //if (g_iInstanceRenderState == 4)
    //    discard;
    //
    //if (Out.vColor.a < g_fDiscardAlpha)
    //    discard;
    
    //Out.vColor = ((vDiffuseMtrl * In.vColor) * vMaskMtrl.r) + vNoiseMtrl;
    //Out.vColor = vDiffuseMtrl * In.vColor;
    //Out.vColor = vDiffuseMtrl;
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCEITEMSLOT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    float4 vDiffuseMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    unsigned int iSlotUV = In.iSlotUV;
    if (iSlotUV == 0) // Top이 걸친 상황
    {
        if (In.vTexcoord.y <= In.vUVRatio.y)
        {
            if (In.iTexIndex == 0)
                Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 1)
                Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 2)
                Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 3)
                Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 4)
                Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 5)
                Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 6)
                Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 7)
                Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 8)
                Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 9)
                Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 10)
                Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 11)
                Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 12)
                Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 13)
                Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 14)
                Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 15)
                Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 16)
                Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 17)
                Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 18)
                Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 19)
                Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 20)
                Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 21)
                Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 22)
                Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 23)
                Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 24)
                Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 25)
                Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 26)
                Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 27)
                Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 28)
                Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 29)
                Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 30)
                Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
        }
        else
            discard;
    }
    else if (iSlotUV == 1) // Bottom이 걸친 상황
    {
        if (In.vTexcoord.y >= In.vUVRatio.y)
        {
            if (In.iTexIndex == 0)
                Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 1)
                Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 2)
                Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 3)
                Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 4)
                Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 5)
                Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 6)
                Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 7)
                Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 8)
                Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 9)
                Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 10)
                Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 11)
                Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 12)
                Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 13)
                Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 14)
                Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 15)
                Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 16)
                Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 17)
                Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 18)
                Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 19)
                Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 20)
                Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 21)
                Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 22)
                Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 23)
                Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 24)
                Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 25)
                Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 26)
                Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 27)
                Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 28)
                Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 29)
                Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 30)
                Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
        }
        else
            discard;
    }
    else if (iSlotUV == 2)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 5)
            Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 6)
            Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 7)
            Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 8)
            Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 9)
            Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 10)
            Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 11)
            Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 12)
            Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 13)
            Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 14)
            Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 15)
            Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 16)
            Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 17)
            Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 18)
            Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 19)
            Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 20)
            Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 21)
            Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 22)
            Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 23)
            Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 24)
            Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 25)
            Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 26)
            Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 27)
            Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 28)
            Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 29)
            Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 30)
            Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (iSlotUV == 3)
        discard;
    
    //Out.vColor = ((vDiffuseMtrl * In.vColor) * vMaskMtrl.r) + vNoiseMtrl;
    //Out.vColor = vDiffuseMtrl * In.vColor;
    
    //if (Out.vColor.a < g_fDiscardAlpha)
    //    discard;
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCEITEMCOOLTIME(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
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
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
        else
        {
            //Out.vColor = float4(1.f, 0.f, 0.f, 1.f) * In.vColor;
        
            discard;
        }
    
        if (Out.vColor.a < g_fDiscardAlpha)
            discard;
	
        return Out;
    }
    
    if (g_iInstanceRenderState == 1)
    {
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
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCEITEMHOVER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
    {
        unsigned int iSlotUV = In.iSlotUV;
        if (iSlotUV == 0) // Top이 걸친 상황
        {
            if (In.vTexcoord.y <= In.vUVRatio.y)
            {
                if (In.iTexIndex == 0)
                    Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
                else if (In.iTexIndex == 1)
                    Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
            }
            else
                discard;
        }
        else if (iSlotUV == 1) // Bottom이 걸친 상황
        {
            if (In.vTexcoord.y >= In.vUVRatio.y)
            {
                if (In.iTexIndex == 0)
                    Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
                else if (In.iTexIndex == 1)
                    Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
            }
            else
                discard;
        }
        else if (iSlotUV == 2)
        {
            if (In.iTexIndex == 0)
                Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 1)
                Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (iSlotUV == 3)
            discard;
    }
    
    if (g_iInstanceRenderState == 1)
    {
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
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCEBG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 5)
            Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 6)
            Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 7)
            Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 8)
            Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 9)
            Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 10)
            Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 11)
            Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 12)
            Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 13)
            Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 14)
            Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 15)
            Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 16)
            Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 17)
            Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 18)
            Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 19)
            Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 20)
            Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 21)
            Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 22)
            Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 23)
            Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 24)
            Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 25)
            Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 26)
            Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 27)
            Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 28)
            Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 29)
            Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 30)
            Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
    }
    
    if (g_iInstanceRenderState == 1)
    {
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
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCEPARRING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
    {
        if (In.vUVRatio.x < 1.f)
        {
            if (In.vTexcoord.y >= In.vUVRatio.y)
            {
                if (In.iTexIndex == 0)
                    Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
                else if (In.iTexIndex == 1)
                    Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
                else if (In.iTexIndex == 2)
                    Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
                else if (In.iTexIndex == 3)
                    Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
                else if (In.iTexIndex == 4)
                    Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
                else if (In.iTexIndex == 5)
                    Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
            }
        }
        
        else
            discard;
    }
    
    if (g_iInstanceRenderState == 1)
    {
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
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCEABNORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
    {
        if (In.iSlotUV == 1)
        {
            if (In.iTexIndex == 0)
                Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 1)
                Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 2)
                Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 3)
                Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 4)
                Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 5)
                Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 6)
                Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 7)
                Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 8)
                Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 9)
                Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 10)
                Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (In.iSlotUV == 0)
            discard;
    }
    
    if (g_iInstanceRenderState == 1)
    {
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
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCEWORLDHP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
    {
        if (In.iSlotUV == 1)
        {
            if (In.vTexcoord.x <= In.vUVRatio.x)
            {
                if (In.iTexIndex == 0)
                    Out.vColor = g_DiffuseTexture0.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 1)
                    Out.vColor = g_DiffuseTexture1.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 2)
                    Out.vColor = g_DiffuseTexture2.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 3)
                    Out.vColor = g_DiffuseTexture3.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 4)
                    Out.vColor = g_DiffuseTexture4.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 5)
                    Out.vColor = g_DiffuseTexture5.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 6)
                    Out.vColor = g_DiffuseTexture6.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 7)
                    Out.vColor = g_DiffuseTexture7.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 8)
                    Out.vColor = g_DiffuseTexture8.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 9)
                    Out.vColor = g_DiffuseTexture9.Sample(g_PointSampler, In.vTexcoord);
                else if (In.iTexIndex == 10)
                    Out.vColor = g_DiffuseTexture10.Sample(g_PointSampler, In.vTexcoord);
            }
            else
                discard;
        }
        else
        {
            if (In.iTexIndex == 0)
                Out.vColor = g_DiffuseTexture0.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 1)
                Out.vColor = g_DiffuseTexture1.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 2)
                Out.vColor = g_DiffuseTexture2.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 3)
                Out.vColor = g_DiffuseTexture3.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 4)
                Out.vColor = g_DiffuseTexture4.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 5)
                Out.vColor = g_DiffuseTexture5.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 6)
                Out.vColor = g_DiffuseTexture6.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 7)
                Out.vColor = g_DiffuseTexture7.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 8)
                Out.vColor = g_DiffuseTexture8.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 9)
                Out.vColor = g_DiffuseTexture9.Sample(g_PointSampler, In.vTexcoord);
            else if (In.iTexIndex == 10)
                Out.vColor = g_DiffuseTexture10.Sample(g_PointSampler, In.vTexcoord);
        }
    }
    
    if (g_iInstanceRenderState == 1)
    {
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
    
    return Out;
}

PS_OUT PS_MAIN_NORMALTEXT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 5)
            Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 6)
            Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 7)
            Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 8)
            Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 9)
            Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 10)
            Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 11)
            Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 12)
            Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 13)
            Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 14)
            Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 15)
            Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 16)
            Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 17)
            Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 18)
            Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 19)
            Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 20)
            Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 21)
            Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 22)
            Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 23)
            Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 24)
            Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 25)
            Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 26)
            Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 27)
            Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 28)
            Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 29)
            Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 30)
            Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
    }
    
    if (g_iInstanceRenderState == 1)
    {
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
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCEQUEST(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_iInstanceRenderState == 0)
    {
        if (In.iSlotUV == 1)
        {
            if (In.iTexIndex == 0)
                Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 1)
                Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 2)
                Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 3)
                Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 4)
                Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 5)
                Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 6)
                Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 7)
                Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 8)
                Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 9)
                Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 10)
                Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 11)
                Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 12)
                Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 13)
                Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 14)
                Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 15)
                Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 16)
                Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 17)
                Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 18)
                Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 19)
                Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 20)
                Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        }
        
        else if (In.iSlotUV == 0)
            discard;
    }
    
    if (g_iInstanceRenderState == 1)
    {
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
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCEQUESTINDICATOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (In.iSlotUV == 0)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
    }
    
    // Mask 적용 만들기
    else if (In.iSlotUV == 1)
        Out.vColor = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord).r * In.vColor;
    
    else
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCECRAFT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (In.iSlotUV == 0)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 5)
            Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 6)
            Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 7)
            Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 8)
            Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 9)
            Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 10)
            Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 11)
            Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 12)
            Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 13)
            Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 14)
            Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 15)
            Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 16)
            Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 17)
            Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 18)
            Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 19)
            Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 20)
            Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 21)
            Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 22)
            Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 23)
            Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 24)
            Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 25)
            Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 26)
            Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 27)
            Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 28)
            Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 29)
            Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 30)
            Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 31)
            Out.vColor = g_DiffuseTexture31.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 32)
            Out.vColor = g_DiffuseTexture32.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 33)
            Out.vColor = g_DiffuseTexture33.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 34)
            Out.vColor = g_DiffuseTexture34.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 35)
            Out.vColor = g_DiffuseTexture35.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 36)
            Out.vColor = g_DiffuseTexture36.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 37)
            Out.vColor = g_DiffuseTexture37.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 38)
            Out.vColor = g_DiffuseTexture38.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 39)
            Out.vColor = g_DiffuseTexture39.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 40)
            Out.vColor = g_DiffuseTexture40.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 41)
            Out.vColor = g_DiffuseTexture41.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 42)
            Out.vColor = g_DiffuseTexture42.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 43)
            Out.vColor = g_DiffuseTexture43.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 44)
            Out.vColor = g_DiffuseTexture44.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 45)
            Out.vColor = g_DiffuseTexture45.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 46)
            Out.vColor = g_DiffuseTexture46.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 47)
            Out.vColor = g_DiffuseTexture47.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 48)
            Out.vColor = g_DiffuseTexture48.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 49)
            Out.vColor = g_DiffuseTexture49.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 50)
            Out.vColor = g_DiffuseTexture50.Sample(g_LinearSampler, In.vTexcoord);
    }
    
    else if (In.iSlotUV == 1)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_INSTANCETRADE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (In.iSlotUV == 0)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 5)
            Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 6)
            Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 7)
            Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 8)
            Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 9)
            Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 10)
            Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 11)
            Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 12)
            Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 13)
            Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 14)
            Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 15)
            Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 16)
            Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 17)
            Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 18)
            Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 19)
            Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 20)
            Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 21)
            Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 22)
            Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 23)
            Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 24)
            Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 25)
            Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 26)
            Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 27)
            Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 28)
            Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 29)
            Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 30)
            Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 31)
            Out.vColor = g_DiffuseTexture31.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 32)
            Out.vColor = g_DiffuseTexture32.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 33)
            Out.vColor = g_DiffuseTexture33.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 34)
            Out.vColor = g_DiffuseTexture34.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 35)
            Out.vColor = g_DiffuseTexture35.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 36)
            Out.vColor = g_DiffuseTexture36.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 37)
            Out.vColor = g_DiffuseTexture37.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 38)
            Out.vColor = g_DiffuseTexture38.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 39)
            Out.vColor = g_DiffuseTexture39.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 40)
            Out.vColor = g_DiffuseTexture40.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 41)
            Out.vColor = g_DiffuseTexture41.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 42)
            Out.vColor = g_DiffuseTexture42.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 43)
            Out.vColor = g_DiffuseTexture43.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 44)
            Out.vColor = g_DiffuseTexture44.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 45)
            Out.vColor = g_DiffuseTexture45.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 46)
            Out.vColor = g_DiffuseTexture46.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 47)
            Out.vColor = g_DiffuseTexture47.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 48)
            Out.vColor = g_DiffuseTexture48.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 49)
            Out.vColor = g_DiffuseTexture49.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 50)
            Out.vColor = g_DiffuseTexture50.Sample(g_LinearSampler, In.vTexcoord);
    }
        
    else if (In.iSlotUV == 1)
        discard;
    
    Out.vColor = Out.vColor * In.vColor;
    
    return Out;
}

PS_OUT PS_MAIN_QUESTCOMPLETE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (In.iSlotUV == 0)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 5)
            Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 6)
            Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 7)
            Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 8)
            Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 9)
            Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 10)
            Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 11)
            Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 12)
            Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 13)
            Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 14)
            Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 15)
            Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 16)
            Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 17)
            Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 18)
            Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 19)
            Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 20)
            Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 21)
            Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 22)
            Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 23)
            Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 24)
            Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 25)
            Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 26)
            Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 27)
            Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 28)
            Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 29)
            Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 30)
            Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 31)
            Out.vColor = g_DiffuseTexture31.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 32)
            Out.vColor = g_DiffuseTexture32.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 33)
            Out.vColor = g_DiffuseTexture33.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 34)
            Out.vColor = g_DiffuseTexture34.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 35)
            Out.vColor = g_DiffuseTexture35.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 36)
            Out.vColor = g_DiffuseTexture36.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 37)
            Out.vColor = g_DiffuseTexture37.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 38)
            Out.vColor = g_DiffuseTexture38.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 39)
            Out.vColor = g_DiffuseTexture39.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 40)
            Out.vColor = g_DiffuseTexture40.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 41)
            Out.vColor = g_DiffuseTexture41.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 42)
            Out.vColor = g_DiffuseTexture42.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 43)
            Out.vColor = g_DiffuseTexture43.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 44)
            Out.vColor = g_DiffuseTexture44.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 45)
            Out.vColor = g_DiffuseTexture45.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 46)
            Out.vColor = g_DiffuseTexture46.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 47)
            Out.vColor = g_DiffuseTexture47.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 48)
            Out.vColor = g_DiffuseTexture48.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 49)
            Out.vColor = g_DiffuseTexture49.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 50)
            Out.vColor = g_DiffuseTexture50.Sample(g_LinearSampler, In.vTexcoord);
    }
    
    else if(In.iSlotUV == 1)
        discard;
    
    Out.vColor = Out.vColor * In.vColor;
    
    return Out;
}

PS_OUT PS_MAIN_ITEMTOOLTIP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    //if (In.iSlotUV == 0)
    //{
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 5)
            Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 6)
            Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 7)
            Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 8)
            Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 9)
            Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 10)
            Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 11)
            Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 12)
            Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 13)
            Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 14)
            Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 15)
            Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 16)
            Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 17)
            Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 18)
            Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 19)
            Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 20)
            Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 21)
            Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 22)
            Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 23)
            Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 24)
            Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 25)
            Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 26)
            Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 27)
            Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 28)
            Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 29)
            Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 30)
            Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 31)
            Out.vColor = g_DiffuseTexture31.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 32)
            Out.vColor = g_DiffuseTexture32.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 33)
            Out.vColor = g_DiffuseTexture33.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 34)
            Out.vColor = g_DiffuseTexture34.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 35)
            Out.vColor = g_DiffuseTexture35.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 36)
            Out.vColor = g_DiffuseTexture36.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 37)
            Out.vColor = g_DiffuseTexture37.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 38)
            Out.vColor = g_DiffuseTexture38.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 39)
            Out.vColor = g_DiffuseTexture39.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 40)
            Out.vColor = g_DiffuseTexture40.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 41)
            Out.vColor = g_DiffuseTexture41.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 42)
            Out.vColor = g_DiffuseTexture42.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 43)
            Out.vColor = g_DiffuseTexture43.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 44)
            Out.vColor = g_DiffuseTexture44.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 45)
            Out.vColor = g_DiffuseTexture45.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 46)
            Out.vColor = g_DiffuseTexture46.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 47)
            Out.vColor = g_DiffuseTexture47.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 48)
            Out.vColor = g_DiffuseTexture48.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 49)
            Out.vColor = g_DiffuseTexture49.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 50)
            Out.vColor = g_DiffuseTexture50.Sample(g_LinearSampler, In.vTexcoord);
    //}
    //else if (In.iSlotUV == 1)
    //    discard;
    
    Out.vColor = Out.vColor * In.vColor;
    
    return Out;
}

PS_OUT PS_MAIN_SPEECHBALLON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    //if (In.iSlotUV == 0)
    //{
    if (In.iTexIndex == 0)
        Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 1)
        Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 2)
        Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 3)
        Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 4)
        Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 5)
        Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 6)
        Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 7)
        Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 8)
        Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 9)
        Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 10)
        Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 11)
        Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 12)
        Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 13)
        Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 14)
        Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 15)
        Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 16)
        Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 17)
        Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 18)
        Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 19)
        Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 20)
        Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 21)
        Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 22)
        Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 23)
        Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 24)
        Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 25)
        Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 26)
        Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 27)
        Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 28)
        Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 29)
        Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 30)
        Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 31)
        Out.vColor = g_DiffuseTexture31.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 32)
        Out.vColor = g_DiffuseTexture32.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 33)
        Out.vColor = g_DiffuseTexture33.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 34)
        Out.vColor = g_DiffuseTexture34.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 35)
        Out.vColor = g_DiffuseTexture35.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 36)
        Out.vColor = g_DiffuseTexture36.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 37)
        Out.vColor = g_DiffuseTexture37.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 38)
        Out.vColor = g_DiffuseTexture38.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 39)
        Out.vColor = g_DiffuseTexture39.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 40)
        Out.vColor = g_DiffuseTexture40.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 41)
        Out.vColor = g_DiffuseTexture41.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 42)
        Out.vColor = g_DiffuseTexture42.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 43)
        Out.vColor = g_DiffuseTexture43.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 44)
        Out.vColor = g_DiffuseTexture44.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 45)
        Out.vColor = g_DiffuseTexture45.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 46)
        Out.vColor = g_DiffuseTexture46.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 47)
        Out.vColor = g_DiffuseTexture47.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 48)
        Out.vColor = g_DiffuseTexture48.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 49)
        Out.vColor = g_DiffuseTexture49.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 50)
        Out.vColor = g_DiffuseTexture50.Sample(g_LinearSampler, In.vTexcoord);
    //}
    //else if (In.iSlotUV == 1)
    //    discard;
    
    Out.vColor = Out.vColor * In.vColor;
    
    return Out;
}

PS_OUT PS_MAIN_STARFORCE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    //if (In.iSlotUV == 0)
    //{
    if (In.iTexIndex == 0)
        Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 1)
        Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 2)
        Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 3)
        Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 4)
        Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 5)
        Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 6)
        Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 7)
        Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 8)
        Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 9)
        Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 10)
        Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 11)
        Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 12)
        Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 13)
        Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 14)
        Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 15)
        Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 16)
        Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 17)
        Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 18)
        Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 19)
        Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 20)
        Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 21)
        Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 22)
        Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 23)
        Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 24)
        Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 25)
        Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 26)
        Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 27)
        Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 28)
        Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 29)
        Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 30)
        Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 31)
        Out.vColor = g_DiffuseTexture31.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 32)
        Out.vColor = g_DiffuseTexture32.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 33)
        Out.vColor = g_DiffuseTexture33.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 34)
        Out.vColor = g_DiffuseTexture34.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 35)
        Out.vColor = g_DiffuseTexture35.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 36)
        Out.vColor = g_DiffuseTexture36.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 37)
        Out.vColor = g_DiffuseTexture37.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 38)
        Out.vColor = g_DiffuseTexture38.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 39)
        Out.vColor = g_DiffuseTexture39.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 40)
        Out.vColor = g_DiffuseTexture40.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 41)
        Out.vColor = g_DiffuseTexture41.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 42)
        Out.vColor = g_DiffuseTexture42.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 43)
        Out.vColor = g_DiffuseTexture43.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 44)
        Out.vColor = g_DiffuseTexture44.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 45)
        Out.vColor = g_DiffuseTexture45.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 46)
        Out.vColor = g_DiffuseTexture46.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 47)
        Out.vColor = g_DiffuseTexture47.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 48)
        Out.vColor = g_DiffuseTexture48.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 49)
        Out.vColor = g_DiffuseTexture49.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 50)
        Out.vColor = g_DiffuseTexture50.Sample(g_LinearSampler, In.vTexcoord);
    //}
    //else if (In.iSlotUV == 1)
    //    discard;
    
    Out.vColor = Out.vColor * In.vColor;
    
    return Out;
}

PS_OUT PS_MAIN_OPTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (In.iSlotUV == 0)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 5)
            Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 6)
            Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 7)
            Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 8)
            Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 9)
            Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 10)
            Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 11)
            Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 12)
            Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 13)
            Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 14)
            Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 15)
            Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 16)
            Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 17)
            Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 18)
            Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 19)
            Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 20)
            Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 21)
            Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 22)
            Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 23)
            Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 24)
            Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 25)
            Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 26)
            Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 27)
            Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 28)
            Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 29)
            Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 30)
            Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 31)
            Out.vColor = g_DiffuseTexture31.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 32)
            Out.vColor = g_DiffuseTexture32.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 33)
            Out.vColor = g_DiffuseTexture33.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 34)
            Out.vColor = g_DiffuseTexture34.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 35)
            Out.vColor = g_DiffuseTexture35.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 36)
            Out.vColor = g_DiffuseTexture36.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 37)
            Out.vColor = g_DiffuseTexture37.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 38)
            Out.vColor = g_DiffuseTexture38.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 39)
            Out.vColor = g_DiffuseTexture39.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 40)
            Out.vColor = g_DiffuseTexture40.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 41)
            Out.vColor = g_DiffuseTexture41.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 42)
            Out.vColor = g_DiffuseTexture42.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 43)
            Out.vColor = g_DiffuseTexture43.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 44)
            Out.vColor = g_DiffuseTexture44.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 45)
            Out.vColor = g_DiffuseTexture45.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 46)
            Out.vColor = g_DiffuseTexture46.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 47)
            Out.vColor = g_DiffuseTexture47.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 48)
            Out.vColor = g_DiffuseTexture48.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 49)
            Out.vColor = g_DiffuseTexture49.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 50)
            Out.vColor = g_DiffuseTexture50.Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (In.iSlotUV == 1)
        discard;
    
    Out.vColor = Out.vColor * In.vColor * g_vColor;
    
    return Out;
}

PS_OUT PS_MAIN_CRAFTRESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (In.iSlotUV == 0)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 5)
            Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 6)
            Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 7)
            Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 8)
            Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 9)
            Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 10)
            Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 11)
            Out.vColor = g_DiffuseTexture11.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 12)
            Out.vColor = g_DiffuseTexture12.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 13)
            Out.vColor = g_DiffuseTexture13.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 14)
            Out.vColor = g_DiffuseTexture14.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 15)
            Out.vColor = g_DiffuseTexture15.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 16)
            Out.vColor = g_DiffuseTexture16.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 17)
            Out.vColor = g_DiffuseTexture17.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 18)
            Out.vColor = g_DiffuseTexture18.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 19)
            Out.vColor = g_DiffuseTexture19.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 20)
            Out.vColor = g_DiffuseTexture20.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 21)
            Out.vColor = g_DiffuseTexture21.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 22)
            Out.vColor = g_DiffuseTexture22.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 23)
            Out.vColor = g_DiffuseTexture23.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 24)
            Out.vColor = g_DiffuseTexture24.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 25)
            Out.vColor = g_DiffuseTexture25.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 26)
            Out.vColor = g_DiffuseTexture26.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 27)
            Out.vColor = g_DiffuseTexture27.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 28)
            Out.vColor = g_DiffuseTexture28.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 29)
            Out.vColor = g_DiffuseTexture29.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 30)
            Out.vColor = g_DiffuseTexture30.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 31)
            Out.vColor = g_DiffuseTexture31.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 32)
            Out.vColor = g_DiffuseTexture32.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 33)
            Out.vColor = g_DiffuseTexture33.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 34)
            Out.vColor = g_DiffuseTexture34.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 35)
            Out.vColor = g_DiffuseTexture35.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 36)
            Out.vColor = g_DiffuseTexture36.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 37)
            Out.vColor = g_DiffuseTexture37.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 38)
            Out.vColor = g_DiffuseTexture38.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 39)
            Out.vColor = g_DiffuseTexture39.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 40)
            Out.vColor = g_DiffuseTexture40.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 41)
            Out.vColor = g_DiffuseTexture41.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 42)
            Out.vColor = g_DiffuseTexture42.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 43)
            Out.vColor = g_DiffuseTexture43.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 44)
            Out.vColor = g_DiffuseTexture44.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 45)
            Out.vColor = g_DiffuseTexture45.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 46)
            Out.vColor = g_DiffuseTexture46.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 47)
            Out.vColor = g_DiffuseTexture47.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 48)
            Out.vColor = g_DiffuseTexture48.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 49)
            Out.vColor = g_DiffuseTexture49.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 50)
            Out.vColor = g_DiffuseTexture50.Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (In.iSlotUV == 1)
        discard;
    
    else if (In.iSlotUV == 2)
    {
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
    
        if (atan2Ret <= standardRatio)
        {
            if (In.iTexIndex == 0)
                Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 1)
                Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 2)
                Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 3)
                Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 4)
                Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 5)
                Out.vColor = g_DiffuseTexture5.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 6)
                Out.vColor = g_DiffuseTexture6.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 7)
                Out.vColor = g_DiffuseTexture7.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 8)
                Out.vColor = g_DiffuseTexture8.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 9)
                Out.vColor = g_DiffuseTexture9.Sample(g_LinearSampler, In.vTexcoord);
            else if (In.iTexIndex == 10)
                Out.vColor = g_DiffuseTexture10.Sample(g_LinearSampler, In.vTexcoord);
        }
            
        else
        {
        //Out.vColor = float4(1.f, 0.f, 0.f, 1.f) * In.vColor;
        
            discard;
        }
    }
    
    Out.vColor = Out.vColor * In.vColor * g_vColor;
    
    return Out;
}

PS_OUT PS_MAIN_SPRITEEFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //if (In.vColor.a == 0.f)
    //    discard;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    //if (In.iSlotUV == 0)
    //{
    //    if (In.iTexIndex == 0)
    //        Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
    //    else if (In.iTexIndex == 1)
    //        Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
    //    else if (In.iTexIndex == 2)
    //        Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
    //    else if (In.iTexIndex == 3)
    //        Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
    //    else if (In.iTexIndex == 4)
    //        Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
    //}
    
    //// Mask 적용 만들기
    //else if (In.iSlotUV == 1)
    //    Out.vColor = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord).r * In.vColor;
    //else
    //    discard;
    
    if (In.iTexIndex == 0)
        Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord).r * In.vColor;
    else if (In.iTexIndex == 1)
        Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 2)
        Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 3)
        Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
    else if (In.iTexIndex == 4)
        Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_MAIN_UVBAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float4 vMaskMtrl = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNoiseMtrl = float4(0.f, 0.f, 0.f, 0.f);
    
    if (In.vTexcoord.x < 0.f || In.vTexcoord.x > 1.f)
        discard;
    
    if (In.vTexcoord.y < 0.f || In.vTexcoord.y > 1.f)
        discard;
    
    if (In.vTexcoord.x <= g_fRatio && In.vTexcoord.x >= g_vTexcoordTL.x)
    {
        if (In.iTexIndex == 0)
            Out.vColor = g_DiffuseTexture0.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 1)
            Out.vColor = g_DiffuseTexture1.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 2)
            Out.vColor = g_DiffuseTexture2.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 3)
            Out.vColor = g_DiffuseTexture3.Sample(g_LinearSampler, In.vTexcoord);
        else if (In.iTexIndex == 4)
            Out.vColor = g_DiffuseTexture4.Sample(g_LinearSampler, In.vTexcoord);
    }
    
    return Out;
}

technique11 DefaultTechnique
{
    pass UI_InstanceItemBG // 0, InstanceItemBG
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEITEMBG();
    }

    pass UI_InstanceItemSlot // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEITEMSLOT();
    }

    pass UI_InstanceItemCoolTime // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEITEMCOOLTIME();
    }

    pass UI_InstanceItemHover // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEITEMHOVER();
    }

    pass UI_InstanceBG // 4, Normal BG(?)
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEBG();
    }

    pass UI_InstanceFrame // 5, Frame
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEBG();
    }

    pass UI_InstanceParring // 6, Parring
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEPARRING();
    }

    pass UI_InstanceAbnormalSlot // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEABNORMAL();
    }

    pass UI_InstanceWorldHP // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEWORLDHP();
    }

    pass UI_NormalText // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMALTEXT();
    }

    pass UI_Quest // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEQUEST();
    }

    pass UI_QuestIndicator // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_QUESTINDICATOR();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCEQUESTINDICATOR();
    }

    pass UI_Craft // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCECRAFT();
    }

    pass UI_Trade // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INSTANCETRADE();
    }

    pass UI_QuestComplete // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_QUESTCOMPLETE();
    }
    
    pass UI_ItemToolTip // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ITEMTOOLTIP();
    }

    pass UI_SpeechBallon // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPEECHBALLON();
    }

    pass UI_StarForce // 17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_STARFORCE();
    }

    pass UI_Option // 18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OPTION();
    }

    pass UI_CraftResult // 19
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CRAFTRESULT();
    }

    pass UI_SpriteEffect // 20
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_QUESTINDICATOR();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPRITEEFFECT();
    }

    pass UI_UVBar // 21
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN_UI_UV();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UVBAR();
    }
}
