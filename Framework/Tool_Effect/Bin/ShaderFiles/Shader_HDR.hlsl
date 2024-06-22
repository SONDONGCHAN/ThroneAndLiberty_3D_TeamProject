#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

Texture2D g_BackBuffer;
Texture2D g_BloomTexture;
Texture2D g_DownSampleTexture;
Texture2D g_LUTTexture;

Texture2D<float4> g_TestTexture;

float g_fTexOffset;
float g_fAspect;

float g_fBlurWeight[9] = { 0.0135f, 0.0476f, 0.1172f,
                           0.2011f, 0.2408f, 0.2011f, 
                           0.1172f, 0.0476f, 0.0135f };

float g_fSaturation, g_fContrast, g_fMiddleGray;

RWTexture2D<float4> g_BlurTexture;

/*Tone Mapping : Filmic TMO*/
/* https://blog.naver.com/PostView.naver?blogId=hblee4119&logNo=222403726749&parentCategoryNo=&categoryNo=6&viewDate=&isShowPopularPosts=false&from=postView */

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
float3x3 aces_input_matrix =
{
    float3(0.59719f, 0.35458f, 0.04823f),
    float3(0.07600f, 0.90834f, 0.01566f),
    float3(0.02840f, 0.13383f, 0.83777f)
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
float3x3 aces_output_matrix =
{
    float3(1.60475f, -0.53108f, -0.07367f),
    float3(-0.10208f, 1.10813f, -0.00605f),
    float3(-0.00327f, -0.07276f, 1.07602f)
};

float3 rtt_and_odt_fit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}


float3 ACESToneMapping(float3 colorIn)
{
    colorIn = mul(aces_input_matrix, colorIn);
    colorIn = rtt_and_odt_fit(colorIn);
    colorIn = mul(aces_output_matrix, colorIn);
    
    return saturate(colorIn);
}


/*LUT ToneMapping*/

float3 GetLutColor(float3 _vColor)
{
    float2 vLutSize = float2(0.00390625f, 0.0625f); // 1 / float(256(color max) , 16(lookup table max))
    float4 vLutUV;
    
    float3 vInColor = saturate(_vColor) * 15.f;
    vLutUV.w = floor(vInColor.b);
    vLutUV.xy = (vInColor.rg + 0.5f) * vLutSize;
    vLutUV.x += vLutUV.w * vLutSize.y;
    vLutUV.z = vLutUV.x + vLutSize.y;
   
    float3 vLerpColor1 = g_LUTTexture.Sample(g_LinearSampler, vLutUV.xyzz).rgb;
    float3 vLerpColor2 = g_LUTTexture.Sample(g_LinearSampler, vLutUV.zyzz).rgb;
    
    return lerp(vLerpColor1, vLerpColor2, vInColor.b - vLutUV.w);
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



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

    vector vBackBuffer = g_BackBuffer.Sample(g_LinearSampler, In.vTexcoord);
    Out.vColor = vector(ACESToneMapping(vBackBuffer.xyz),1.f);
        
	return Out;
}


/*5x5 DownSamlping*/
float4 PS_DOWN_SAMPLE(PS_IN In) : SV_TARGET0
{
    PS_OUT Out = (PS_OUT) 0;	
    float3 vDownScaled = float3(0.f, 0.f, 0.f);

    [unroll]
    for (int i = 0; i < 5; ++i)
    {
        [unroll]
        for (int j = 0; j < 5; ++j)
        {
            vDownScaled += g_BackBuffer.Sample(g_LinearClampSampler, In.vTexcoord, int2(j, i)).rgb;
        }

    }

    vDownScaled *= 0.04f;
	
    return float4(vDownScaled, 1.f);
}

/*5x5 DownSamlping + Return LumAvg*/
float4 PS_DOWN_SAMPLE_LUM(PS_IN In) : SV_TARGET0
{
    PS_OUT Out = (PS_OUT) 0;
    float3 vDownScaled = float3(0.f, 0.f, 0.f);
	
    //In.vTexcoord *= 5.f;
    
    [unroll]
    for (int i = 0; i < 5; ++i)
    {
        [unroll]
        for (int j = 0; j < 5; ++j)
        {
            vDownScaled += g_BackBuffer.Sample(g_LinearClampSampler, In.vTexcoord, int2(i, j)).rgb;
        }

    }
   
    vDownScaled *= 0.04f;
    float3 vLumAvg = exp(vDownScaled);
    return float4(vLumAvg, 1.f);
}



float4 PS_TONEMAPPING(PS_IN In) : SV_TARGET0
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_BackBuffer.Sample(g_LinearSampler, In.vTexcoord);
    //vColor = pow(vColor, 1.f / 2.2f);
    
    
    vColor *= 2.f * g_fSaturation;
	
    float avg = (vColor.r + vColor.g + vColor.b) / 3.0;
    float4 vNewColor;
    vNewColor.a = 1.0f;
    vNewColor.rgb = avg * (2.f * g_fMiddleGray - 1.f) + vColor.rgb * 2.f * (1.f - g_fMiddleGray);
    
    float4 vFinColor = 0.5f + 1.4f * g_fContrast * (vNewColor - 0.5f);
            
    
    //vFinColor = pow(vFinColor, 0.45f);
    //float3 vToneMapColor = ACESToneMapping(vColor.xyz);
    
    //return float4(vToneMapColor, 1.f);
    
    return vFinColor;

}

float4 PS_HDR_BLURV(PS_IN In) :SV_Target0
{
    float4 vColor = Blur_X(In.vTexcoord, g_DownSampleTexture);
    return vColor;
}

float4 PS_HDR_BLURH(PS_IN In) : SV_Target0
{
    
    float4 vColor = Blur_Y(In.vTexcoord, g_DownSampleTexture);
    return vColor;
}


float4 PS_UP_SAMPLE(PS_IN In) :SV_Target0
{
    float3 outColor = float3(0.0f, 0.0f, 0.0f);
        
    float3 vDst = g_BloomTexture.Sample(g_LinearClampSampler, In.vTexcoord * float2(1.f, (g_fAspect))).rgb;
    float3 vSrc = g_DownSampleTexture.Sample(g_LinearClampSampler, In.vTexcoord).rgb;
    
    /*Gamma Collect*/
    outColor = pow(pow(abs(vDst), 2.2f) + pow(abs(vSrc), 2.2f), 1.f / 2.2f);
    
    return float4(outColor, 1.0f);
    
}

float4 PS_UP_SAMPLE2(PS_IN In) : SV_Target0
{
    float3 outColor = float3(0.0f, 0.0f, 0.0f);
    float3 vSrc = g_DownSampleTexture.Sample(g_LinearClampSampler, In.vTexcoord).rgb;
    
    outColor = pow(pow(abs(vSrc), 2.2f), 1.f / 2.2f);
    
    return float4(outColor, 1.0f);
    
}


float4 PS_LUMINANCE(PS_IN In) : SV_Target0
{
    
    float4 fBrightColor = 0.f;
    float4 vFragColor = g_BackBuffer.Sample(g_LinearSampler, In.vTexcoord);
    
    float fBrightness = dot(vFragColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    
    if (fBrightness > 0.98f)
        fBrightColor = float4(vFragColor.rgb, 1.f);
    
    return fBrightColor;
    
}


#define GAUSSIAN_RADIUS 4

float4 PS_MAIN_BLOOMBLUR_H(VS_OUT In) : SV_Target0
{
    float4 vFinalPixel = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float2 vTexOffset;

    vTexOffset = float2(g_fTexOffset, 0.f);

    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = -GAUSSIAN_RADIUS; i <= GAUSSIAN_RADIUS; ++i)
    {
        float2 vTexcoord = In.vTexcoord + i * vTexOffset;
        float4 vSample = g_DownSampleTexture.Sample(g_LinearBorderSampler, vTexcoord);
        vFinalPixel.a += vSample.a * g_fBlurWeight[i + GAUSSIAN_RADIUS] * 1.5f;
    }

    return vFinalPixel;
}


float4 PS_MAIN_BLOOMBLUR_V(VS_OUT In) : SV_Target0
{
    float4 vFinalPixel = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float2 vTexOffset;

    vTexOffset = float2(0.0f, g_fTexOffset);
    
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = -GAUSSIAN_RADIUS; i <= GAUSSIAN_RADIUS; ++i)
    {
        float2 vTexcoord = In.vTexcoord + i * vTexOffset;
        float4 vSample = g_DownSampleTexture.Sample(g_LinearBorderSampler, vTexcoord);
 
        vFinalPixel.a += vSample.a * g_fBlurWeight[i + GAUSSIAN_RADIUS] * 1.5f;
    }
    
 //   clip(vFinalPixel.a - 0.03f);
    
    return vFinalPixel;
}


[numthreads(16, 16, 1)]
void CS_BLUR(uint3 id : SV_DispatchThreadID)
{  
    float4 OriginColor = g_BackBuffer.Load(id.xyz);
    g_BlurTexture[id.xy] = (OriginColor.r + OriginColor.g + OriginColor.b) / 3.f;

}

float4 PS_CS_TEST(PS_IN In) : SV_Target0
{
    
    float4 vColor = g_TestTexture.Sample(g_LinearSampler, In.vTexcoord);
    //g_BackBuffer.Sample(g_LinearSampler, In.vTexcoord);
    return vColor;
}

technique11	DefaultTechnique 
{
	pass Default //0
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass DownSample //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_DOWN_SAMPLE();
    }

    pass ToneMap //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_TONEMAPPING();
    }

    pass UpSample //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_UP_SAMPLE();
    }
    
    /*Horizontal*/
    pass BloomBlurH //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOMBLUR_H();
    }

    /*Vertical*/
    pass BloomBlurV //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOMBLUR_V();
    }

    pass Luminance //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_LUMINANCE();
    }

    pass Upsampling2 //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_UP_SAMPLE2();
    }

    pass CSBlur //8
    {
        ComputeShader = compile cs_5_0 CS_BLUR();
    }

    pass CSTest // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_CS_TEST();
    }


}


