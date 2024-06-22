#include "Shader_Defines.hlsli"

Texture2D g_Input;
RWTexture2D<float4> g_Output;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

float g_fScreenWidth;
float g_fScreenHeight;

uint g_TextureSize;

/*Horizontal Blur*/
[numthreads(16, 16, 1)]
void CS_BLUR_X(uint3 id : SV_DispatchThreadID)
{
    float4 vOut = (float4) 0;

    uint2 vUV = (float2) 0;
    float fTotal = 0.f;
    int iValue = (g_iLowWeights - 1) / 2;
    
    for (int i = -iValue; i <= iValue; ++i)
    {
        vUV = id.xy + float2(i, 0.f);

        if (id.x % g_TextureSize < iValue)
        {
            uint iTexArrX = min(id.x + i, g_TextureSize - 1);
            vUV = uint2(iTexArrX, id.y);
        }
    
        if (id.x % g_TextureSize > g_TextureSize - iValue)
        {
            uint iTexArrX = max(id.x - i, 0);

            vUV = uint2(iTexArrX, id.y);
        }
        
        float weight = g_fLowWeight[iValue + i];
        vOut += g_Input.Load(float3(vUV, id.z)) * weight;
        fTotal += weight;
    
    }
        
    vOut /= fTotal;
    g_Output[id.xy] = vOut;
}


/*Down Sampling*/
[numthreads(16, 16, 1)]
void CS_DOWNSAMPLE(uint3 id : SV_DispatchThreadID)
{
    float3 vDownScaled = float3(0.f, 0.f, 0.f);
    float4 vOut;
    
    int3 vUV = int3(id.xy * 5, id.z);
    
    for (int i = 0; i < 5; ++i)
    {
        
        for (int j = 0; j < 5; ++j)
        {
            vDownScaled += g_Input.Load(vUV, int2(j, i));
        }
      
    }
        
    vDownScaled *= 0.04f;
    g_Output[id.xy] = float4(vDownScaled, 1.f);

}



/*Vertical Blur*/
[numthreads(16, 16, 1)]
void CS_BLUR_Y(uint3 id : SV_DispatchThreadID)
{
    float4 vOut = (float4) 0;

    uint2 vUV = (float2) 0;
    float fTotal = 0.f;
    int iValue = (g_iLowWeights - 1) / 2;
    
    for (int i = -iValue; i <= iValue; ++i)
    {
        vUV = id.xy + float2(0.f, i);

        if (id.y % g_TextureSize < iValue)
        {
            uint iTexArrY = max(id.y - i, 0);
            vUV = uint2(id.x, iTexArrY);
           
        }
    
        if (id.y % g_TextureSize > g_TextureSize - iValue)
        {
            
            uint iTexArrY = min(id.y + i, g_TextureSize - 1);
            vUV = uint2(id.x, iTexArrY);
         
          
        }
        
        float weight = g_fLowWeight[iValue + i];
        vOut += g_Input.Load(float3(vUV, id.z)) * weight;
        fTotal += weight;
    
    }
        
    vOut /= fTotal;
    g_Output[id.xy] = vOut;
}

/*UpSampling*/
[numthreads(16, 16, 1)]
void CS_UPSAMPLE(uint3 id : SV_DispatchThreadID)
{
    float3 vDownScaled = float3(0.f, 0.f, 0.f);
    float4 vOut;
    
    int3 vUV = int3(id.xy * 5, id.z);
    
    for (int i = 0; i < 5; ++i)
    {
        
        for (int j = 0; j < 5; ++j)
        {
            vDownScaled += g_Input.Load(vUV, int2(j, i));
        }
      
    }
        
    vDownScaled *= 0.04f;
    g_Output[id.xy] = float4(vDownScaled, 1.f);

}


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
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
	
    return Out;
}

technique11 DefaultTechnique
{
    pass Default //0 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = NULL;
    }

    pass CSBlurX //1
    {
        ComputeShader = compile cs_5_0 CS_BLUR_X();
    }

    pass CSBlurY //2
    {
        ComputeShader = compile cs_5_0 CS_BLUR_Y();
    }

    pass CSDownSample //3
    {
        ComputeShader = compile cs_5_0 CS_DOWNSAMPLE();
    }

}