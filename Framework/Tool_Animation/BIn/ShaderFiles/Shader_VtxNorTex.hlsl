#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

texture2D g_BrushTexture;
float3 g_vBrushPos = float3(1.f, 0.f, 1.f);
float g_fBrushRadius = 1.f;

uint g_iNumWeightTexture = 0;

texture2D g_BlendTexture_Red_0;
texture2D g_BlendTexture_Green_0;
texture2D g_BlendTexture_Blue_0;
texture2D g_BlendTexture_Alpha_0;

texture2D g_BlendTexture_Red_1;
texture2D g_BlendTexture_Green_1;
texture2D g_BlendTexture_Blue_1;
texture2D g_BlendTexture_Alpha_1;

texture2D g_BlendTexture_Red_2;
texture2D g_BlendTexture_Green_2;
texture2D g_BlendTexture_Blue_2;
texture2D g_BlendTexture_Alpha_2;

texture2D g_BlendTexture_Red_3;
texture2D g_BlendTexture_Green_3;
texture2D g_BlendTexture_Blue_3;
texture2D g_BlendTexture_Alpha_3;

texture2D g_BlendTexture_Red_4;
texture2D g_BlendTexture_Green_4;
texture2D g_BlendTexture_Blue_4;
texture2D g_BlendTexture_Alpha_4;

texture2D g_WeightTexture_0;
texture2D g_WeightTexture_1;
texture2D g_WeightTexture_2;
texture2D g_WeightTexture_3;
texture2D g_WeightTexture_4;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;

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
    
    return Out;

}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //vector vSourDiffuse = g_DiffuseTexture.Sample(g_PointSampler, In.vTexcoord);

    //Out.vDiffuse = vector(vSourDiffuse.rgb, 1.f);
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

    vector vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
    vector vMtrlDiffuse = float4(0, 0, 0, 0);
    float fDiffuseAlpha = 1.f;
    
    if (1 <= g_iNumWeightTexture)
    {
        
        vector vBlendTexture_Red_0 = g_BlendTexture_Red_0.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_0 = g_BlendTexture_Green_0.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_0 = g_BlendTexture_Blue_0.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_0 = g_BlendTexture_Alpha_0.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_0 = g_WeightTexture_0.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse = (vBlendTexture_Red_0 * vWeightTexture_0.r) +
                            (vBlendTexture_Green_0 * vWeightTexture_0.g) +
                            (vBlendTexture_Blue_0 * vWeightTexture_0.b) +
                            (vBlendTexture_Alpha_0 * vWeightTexture_0.a);
         
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_0.r - vWeightTexture_0.g - vWeightTexture_0.b - vWeightTexture_0.a), 0.f);
    }
    
    if (2 <= g_iNumWeightTexture)
    {
        vector vBlendTexture_Red_1 = g_BlendTexture_Red_1.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_1 = g_BlendTexture_Green_1.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_1 = g_BlendTexture_Blue_1.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_1 = g_BlendTexture_Alpha_1.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_1 = g_WeightTexture_1.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse += (vBlendTexture_Red_1 * vWeightTexture_1.r) +
                            (vBlendTexture_Green_1 * vWeightTexture_1.g) +
                            (vBlendTexture_Blue_1 * vWeightTexture_1.b) +
                            (vBlendTexture_Alpha_1 * vWeightTexture_1.a);
        
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_1.r - vWeightTexture_1.g - vWeightTexture_1.b - vWeightTexture_1.a), 0.f);
    }
    
    if (3 <= g_iNumWeightTexture)
    {
        vector vBlendTexture_Red_2 = g_BlendTexture_Red_2.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_2 = g_BlendTexture_Green_2.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_2 = g_BlendTexture_Blue_2.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_2 = g_BlendTexture_Alpha_2.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_2 = g_WeightTexture_2.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse += (vBlendTexture_Red_2 * vWeightTexture_2.r) +
                            (vBlendTexture_Green_2 * vWeightTexture_2.g) +
                            (vBlendTexture_Blue_2 * vWeightTexture_2.b) +
                            (vBlendTexture_Alpha_2 * vWeightTexture_2.a);
        
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_2.r - vWeightTexture_2.g - vWeightTexture_2.b - vWeightTexture_2.a), 0.f);
    }
    
    if (4 <= g_iNumWeightTexture)
    {
        vector vBlendTexture_Red_3 = g_BlendTexture_Red_3.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_3 = g_BlendTexture_Green_3.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_3 = g_BlendTexture_Blue_3.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_3 = g_BlendTexture_Alpha_3.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_3 = g_WeightTexture_3.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse += (vBlendTexture_Red_3 * vWeightTexture_3.r) +
                            (vBlendTexture_Green_3 * vWeightTexture_3.g) +
                            (vBlendTexture_Blue_3 * vWeightTexture_3.b) +
                            (vBlendTexture_Alpha_3 * vWeightTexture_3.a);
        
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_3.r - vWeightTexture_3.g - vWeightTexture_3.b - vWeightTexture_3.a), 0.f);
    }
    
    if (5 <= g_iNumWeightTexture)
    {
        vector vBlendTexture_Red_4 = g_BlendTexture_Red_4.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_4 = g_BlendTexture_Green_4.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_4 = g_BlendTexture_Blue_4.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_4 = g_BlendTexture_Alpha_4.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_4 = g_WeightTexture_4.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse += (vBlendTexture_Red_4 * vWeightTexture_4.r) +
                            (vBlendTexture_Green_4 * vWeightTexture_4.g) +
                            (vBlendTexture_Blue_4 * vWeightTexture_4.b) +
                            (vBlendTexture_Alpha_4 * vWeightTexture_4.a);
        
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_4.r - vWeightTexture_4.g - vWeightTexture_4.b - vWeightTexture_4.a), 0.f);
    }
    
    Out.vDiffuse = fDiffuseAlpha * vDiffuse + vMtrlDiffuse;

    Out.vDiffuse.a = 1.f;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_TOOL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    

    /*
    vector vBlendTexture_Red = g_BlendTexture_Red.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    vector vBlendTexture_Green = g_BlendTexture_Green.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    vector vBlendTexture_Blue = g_BlendTexture_Blue.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    vector vBlendTexture_Alpha = g_BlendTexture_Alpha.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
    vector vWeightTexture = g_WeightTexture.Sample(g_LinearSampler, In.vTexcoord);
    */
    
    vector vMtrlDiffuse = float4(0, 0, 0, 0);
    float fDiffuseAlpha = 1.f;
    
    if (1 <= g_iNumWeightTexture)
    {
        
        vector vBlendTexture_Red_0 = g_BlendTexture_Red_0.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_0 = g_BlendTexture_Green_0.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_0 = g_BlendTexture_Blue_0.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_0 = g_BlendTexture_Alpha_0.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_0 = g_WeightTexture_0.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse = (vBlendTexture_Red_0 * vWeightTexture_0.r) +
                            (vBlendTexture_Green_0 * vWeightTexture_0.g) +
                            (vBlendTexture_Blue_0 * vWeightTexture_0.b) +
                            (vBlendTexture_Alpha_0 * vWeightTexture_0.a);
         
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_0.r - vWeightTexture_0.g - vWeightTexture_0.b - vWeightTexture_0.a), 0.f);
    }
    
    if (2 <= g_iNumWeightTexture)
    {
        vector vBlendTexture_Red_1 = g_BlendTexture_Red_1.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_1 = g_BlendTexture_Green_1.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_1 = g_BlendTexture_Blue_1.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_1 = g_BlendTexture_Alpha_1.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_1 = g_WeightTexture_1.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse += (vBlendTexture_Red_1 * vWeightTexture_1.r) +
                            (vBlendTexture_Green_1 * vWeightTexture_1.g) +
                            (vBlendTexture_Blue_1 * vWeightTexture_1.b) +
                            (vBlendTexture_Alpha_1 * vWeightTexture_1.a);
        
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_1.r - vWeightTexture_1.g - vWeightTexture_1.b - vWeightTexture_1.a), 0.f);
    }
    
    if (3 <= g_iNumWeightTexture)
    {
        vector vBlendTexture_Red_2 = g_BlendTexture_Red_2.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_2 = g_BlendTexture_Green_2.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_2 = g_BlendTexture_Blue_2.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_2 = g_BlendTexture_Alpha_2.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_2 = g_WeightTexture_2.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse += (vBlendTexture_Red_2 * vWeightTexture_2.r) +
                            (vBlendTexture_Green_2 * vWeightTexture_2.g) +
                            (vBlendTexture_Blue_2 * vWeightTexture_2.b) +
                            (vBlendTexture_Alpha_2 * vWeightTexture_2.a);
        
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_2.r - vWeightTexture_2.g - vWeightTexture_2.b - vWeightTexture_2.a), 0.f);
    }
    
    if (4 <= g_iNumWeightTexture)
    {
        vector vBlendTexture_Red_3 = g_BlendTexture_Red_3.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_3 = g_BlendTexture_Green_3.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_3 = g_BlendTexture_Blue_3.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_3 = g_BlendTexture_Alpha_3.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_3 = g_WeightTexture_3.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse += (vBlendTexture_Red_3 * vWeightTexture_3.r) +
                            (vBlendTexture_Green_3 * vWeightTexture_3.g) +
                            (vBlendTexture_Blue_3 * vWeightTexture_3.b) +
                            (vBlendTexture_Alpha_3 * vWeightTexture_3.a);
        
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_3.r - vWeightTexture_3.g - vWeightTexture_3.b - vWeightTexture_3.a), 0.f);
    }
    
    if (5 <= g_iNumWeightTexture)
    {
        vector vBlendTexture_Red_4 = g_BlendTexture_Red_4.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Green_4 = g_BlendTexture_Green_4.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Blue_4 = g_BlendTexture_Blue_4.Sample(g_LinearSampler, In.vTexcoord * 30.f);
        vector vBlendTexture_Alpha_4 = g_BlendTexture_Alpha_4.Sample(g_LinearSampler, In.vTexcoord * 30.f);
    
        vector vWeightTexture_4 = g_WeightTexture_4.Sample(g_LinearSampler, In.vTexcoord);
        
        vMtrlDiffuse += (vBlendTexture_Red_4 * vWeightTexture_4.r) +
                            (vBlendTexture_Green_4 * vWeightTexture_4.g) +
                            (vBlendTexture_Blue_4 * vWeightTexture_4.b) +
                            (vBlendTexture_Alpha_4 * vWeightTexture_4.a);
        
        fDiffuseAlpha = max((fDiffuseAlpha - vWeightTexture_4.r - vWeightTexture_4.g - vWeightTexture_4.b - vWeightTexture_4.a), 0.f);
    }
    
    
    vector vBrush = vector(0.f, 0.f, 0.f, 0.f);

    if (g_vBrushPos.x - g_fBrushRadius < In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRadius &&
		g_vBrushPos.z - g_fBrushRadius < In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRadius)
    {
        float2 fNewUV;

        fNewUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRadius)) / (2.f * g_fBrushRadius);
        fNewUV.y = ((g_vBrushPos.z + g_fBrushRadius) - In.vWorldPos.z) / (2.f * g_fBrushRadius);

        vBrush = g_BrushTexture.Sample(g_LinearSampler, fNewUV);
    }

    Out.vDiffuse = fDiffuseAlpha * vDiffuse + vMtrlDiffuse + vBrush;

    //Out.vDiffuse = max((1 - vWeightTexture.r - vWeightTexture.g - vWeightTexture.b - vWeightTexture.a), 0) * vDiffuse + vMtrlDiffuse + vBrush;
    
    Out.vDiffuse.a = 1.f;
    //Out.vDiffuse = vector(vDiffuse.rgb, 1.f);
    //Out.vDiffuse = vector(vSourDiffuse.rgb, 1.f) + vBrush;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

    return Out;
}

struct PS_PIXELPICKING_OUT
{
    //for. Pixel Picking
    vector vPixelPicking : SV_TARGET0;
    vector vPixelPickingView : SV_TARGET1;
};

PS_PIXELPICKING_OUT PS_PIXELPICKING(PS_IN In)
{
    PS_PIXELPICKING_OUT Out = (PS_PIXELPICKING_OUT) 0;
    
    //for. Pixel Picking
    Out.vPixelPicking = float4(In.vWorldPos);
    Out.vPixelPickingView = float4(In.vWorldPos.x / 257.f, (In.vWorldPos.y / 257.f) * 5.f, In.vWorldPos.z / 257.f, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        //SetRasterizerState(RS_WireFrame);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Tool //1
    {
        SetRasterizerState(RS_Cull_None);
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_TOOL();
    }

    pass PixelPicking //2
    {
        //SetRasterizerState(RS_WireFrame);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_PIXELPICKING();
    }

    pass ToolWireFrame //3
    {
        SetRasterizerState(RS_WireFrame);
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_TOOL();
    }
}