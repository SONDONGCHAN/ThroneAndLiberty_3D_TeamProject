#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix		g_ViewMatrixInv, g_ProjMatrixInv;

matrix		g_PrevViewMatrix, g_PerspectiveProjMat;
float		g_fGlowPower = 2.2f;

float		g_fDistortionStrength = 1.f;
float		g_fMotionBlurPower;

texture2D	g_Texture;
texture2D	g_BackBufferTexture;
texture2D	g_BlendTexture;
texture2D	g_EffectTexture;
texture2D	g_BlurTexture;
texture2D	g_DistortionTexture;

vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector		g_vCamPosition;

texture2D	g_NormalTexture;
texture2D	g_DiffuseTexture;
texture2D	g_ShadeTexture;
texture2D	g_DepthTexture;
texture2D	g_EdgeTexture;
texture2D	g_SpecularTexture;

float2		g_vZoomPoint = { 0.5f, 0.5f };
float		g_fZoomBlurPower;

float		g_fFadeRate = 1.f;
float		g_fDeltaTime = 0.f;


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

struct VS_BLUR_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord[30] : TEXCOORD;
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

VS_BLUR_OUT VS_MAIN_BLUR_X(VS_IN In)
{
	VS_BLUR_OUT Out = (VS_BLUR_OUT)0;

	float texelSize;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vTexcoord[0] = In.vTexcoord;

	texelSize = 1.0f / 1280.f;

	for (int i = -14; i < 15; ++i)
		Out.vTexcoord[i + 15] = In.vTexcoord + float2(texelSize * (float)i, 0.0f);

	return Out;
}

VS_BLUR_OUT VS_MAIN_BLUR_Y(VS_IN In)
{
	VS_BLUR_OUT Out = (VS_BLUR_OUT)0;

	float texelSize;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vTexcoord[0] = In.vTexcoord;

	texelSize = 1.0f / 720.f;

	[unroll]
	for (int i = -14; i < 15; ++i)
		Out.vTexcoord[i + 15] = In.vTexcoord + float2(0.0f, texelSize * (float)i);

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

struct PS_BLUR_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord[30] : TEXCOORD;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	// 백버퍼
	float4		vBackBufferCopy = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);

	// Y블러까지 마친 블러 텍스처
	float4		vBlendTexture = g_BlendTexture.Sample(g_LinearSampler, In.vTexcoord);

	// 글로우 재료 텍스처
	vector		vBlurTexture = g_BlurTexture.Sample(g_LinearSampler, In.vTexcoord);

    Out.vColor = vBackBufferCopy + vBlendTexture + vBlurTexture;

	//Out.vColor = pow(pow(abs(vBackBufferCopy), g_fGlowPower) + pow(abs(vBlendTexture), g_fGlowPower), 1.f / g_fGlowPower);
	//Out.vColor = pow(pow(abs(vBackAndGlow), g_fGlowPower) + pow(abs(vBlurTexture), g_fGlowPower), 1.f / g_fGlowPower);
	//Out.vColor = vBackAndGlow + vBlurTexture;

	/*if ((vBlendTexture.r != 0.f || vBlendTexture.g != 0.f || vBlendTexture.b != 0.f) && vBlendTexture.a == 1.f)
		Out.vColor = vBlendTexture;*/

	return Out;
}

//PS_OUT PS_MAIN_OUTLINE(PS_IN In)
//{
//	PS_OUT Out = (PS_OUT)0;
//
//	// 백버퍼
//	float4		vBackBufferCopy = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);
//
//	// 뎁스 텍스처
//	float4		vDepthTexture = g_DepthTexture.Sample(g_LinearSampler, In.vTexcoord);
//
//	// 외곽선 텍스처
//	vector		vOutlineTexture = g_EdgeTexture.Sample(g_LinearSampler, In.vTexcoord);
//
//	Out.vColor = vBackBufferCopy;
//
//	if (vOutlineTexture.x > 0.f)
//		Out.vColor = (1.f, 0.f, 1.f, 1.f);
//
//	return Out;
//}

PS_OUT PS_MAIN_OUTLINE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	// 백버퍼
	float4		vBackBufferCopy = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);

	// 외곽선 텍스처
	vector		vOutlineTexture = g_EdgeTexture.Sample(g_LinearSampler, In.vTexcoord);

	// 뎁스 텍스처
	float4		vDepthTexture = g_DepthTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vDepthTexture.x >= 1.f && vOutlineTexture.z <= 1.f)
        vBackBufferCopy.xyz += vOutlineTexture.xyz;

	Out.vColor = float4(vBackBufferCopy.xyz, 1.f);

	return Out;
}

PS_OUT PS_MAIN_BLUR(PS_BLUR_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float normalization;
	float4 color;



	/*float fWeight[15] = {
		1.0f,
		0.9615f, 
		0.9231f,
		0.8246f, 
		0.7261f,
		0.60645f,
		0.4868f,
		0.3824f, 
		0.278f,
		0.20665f,
		0.1353f,
		0.0957f, 
		0.0561f,
		0.0324f, 
		0.01805f
	};*/

	float fWeight[15] = {
		1.0f, 0.9231f, 0.7261f, 0.4868f, 0.278f, 0.1353f, 0.0561f,
		0.025f, 0.015f, 0.008f, 0.004f, 0.002f, 0.001f, 0.0005f, 0.0002f
	};

	// float fWeight[7] = { 1.0f, 0.9231f, 0.7261f, 0.4868f, 0.278f, 0.1353f, 0.0561f };

	normalization = fWeight[0];
	for (int i = 1; i < 7; ++i)
		normalization += fWeight[i] * 2.f;

	for (int n = 0; n < 7; ++n)
		fWeight[n] /= ((normalization));	
	
	/*for (int i = 0; i < 7; ++i)
		fWeight[i] = fWeight[i] / normalization;*/

	color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]
    for (int j = -7; j < 8; ++j)
	{
        In.vTexcoord[8 + j].x = saturate(In.vTexcoord[8 + j].x);
		color += (g_BlurTexture.Sample(g_LinearClampSampler, In.vTexcoord[8 + j])) * fWeight[abs(j)];
	}

	color.a = 1.0f;

	Out.vColor = color;

	return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_BLUR_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float normalization;
	float4 color;

	float fWeight[7] = { 1.0f, 0.9231f, 0.7261f, 0.4868f, 0.278f, 0.1353f, 0.0561f };

	normalization = (fWeight[0] + 2.0f * (fWeight[1] + fWeight[2] + fWeight[3] + fWeight[4] + fWeight[5] + fWeight[6]));

	for (int i = 0; i < 7; ++i)
		fWeight[i] = fWeight[i] / normalization;

	//weight0 = 1.0f;
	//weight1 = 0.9f;
	//weight2 = 0.55f;
	//weight3 = 0.18f;
	//weight4 = 0.1f;

	color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (int j = -6; j < 7; ++j)
		color += g_BlurTexture.Sample(g_PointSampler, In.vTexcoord[7 + j]) * fWeight[abs(j)];

	color.a = 1.0f;

	Out.vColor = color;

	return Out;
}

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4		vDistortionTexture = g_DistortionTexture.Sample(g_LinearSampler, In.vTexcoord);

	// 디스토션이 적용되는 픽셀
	if (vDistortionTexture.a > 0.f)
	{
		float2		vNoisedUVs = In.vTexcoord + vDistortionTexture.r;

		In.vTexcoord = lerp(In.vTexcoord, vNoisedUVs, vDistortionTexture.a / 100);

		Out.vColor = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);
	}

	else
		Out.vColor = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
}

float4 PS_MOTION_BLUR(PS_IN In) :SV_Target0
{
    PS_OUT Out = (PS_OUT) 0;
	
	
    float4 vDepthTexture = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);
		   
	
    if (0.f == vDepthTexture.y)
    {
        float4 currentColor = g_BackBufferTexture.Sample(g_LinearClampSampler, In.vTexcoord);
        return currentColor;

    }
    else
    {
    
        float fViewZ = vDepthTexture.y * 1000.f;
		    
        float4 vWorldPos, vPixelPos;
	
        vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
        vWorldPos.y = (1 - In.vTexcoord.y) * 2.f - 1.f;
        vWorldPos.z = vDepthTexture.x;
        vWorldPos.w = 1.f;

        vPixelPos = vWorldPos;
        vWorldPos *= fViewZ;

        matrix InverseMat = mul(g_ProjMatrixInv, g_ViewMatrixInv);
        vWorldPos = mul(vWorldPos, InverseMat);
	
        matrix matVP = mul(g_PrevViewMatrix, g_PerspectiveProjMat);

        float4 vPrePixelPos = mul(vWorldPos, matVP);
        vPrePixelPos /= vPrePixelPos.w;

        float2 vPixelVelocity = ((vPixelPos - vPrePixelPos) * 0.5f).xy;
        float2 texCoord = In.vTexcoord;

        float4 vColor = vector(0.f, 0.f, 0.f, 0.f);

        for (uint i = 1; i <= 10; ++i)
        {
            texCoord += vPixelVelocity * g_fMotionBlurPower * i;
            float4 currentColor = g_BackBufferTexture.Sample(g_LinearClampSampler, texCoord);
            vColor += currentColor;
        }
		
        return vColor / 10.f;

    }

}

float4 PS_CHROMATIC_ABERRATION(PS_IN In) :SV_Target0
{
    float fROffset = 0.007f;
    float fGOffset = 0.000f;
    float fBOffset = -0.009f;
	
    float3 vBackColor = 0.f;
	
    vBackColor.r = (g_BackBufferTexture.Sample(g_LinearClampSampler, In.vTexcoord + fROffset)).r;
    vBackColor.g = (g_BackBufferTexture.Sample(g_LinearClampSampler, In.vTexcoord + fGOffset)).g;
    vBackColor.b = (g_BackBufferTexture.Sample(g_LinearClampSampler, In.vTexcoord + fBOffset)).b;
	
    return float4(vBackColor, 1.f);
	
}

PS_OUT PS_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vBackBuffer = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vBlendBuffer = g_BlendTexture.Sample(g_LinearSampler, In.vTexcoord);
	
    Out.vColor = vBackBuffer + vBlendBuffer;

    return Out;
}

float4 PS_ZOOM_BLUR(PS_IN In) :SV_Target0
{
	PS_OUT Out = (PS_OUT)0;

	// g_vZoomPoint : -1 ~ 1 사이의 스크린 투영 좌표를 0~1의 texcoord로 변환한 것
	// In.vTexcoord : 현재 픽셀의 텍스처 좌표

	float2 vDiff = ((In.vTexcoord - g_vZoomPoint) * 0.5f);
	float2 vUV = In.vTexcoord;

	float4 vColor = vector(0.f, 0.f, 0.f, 0.f);

	for (uint i = 1; i <= 10; ++i)
	{
		vUV += vDiff * g_fZoomBlurPower * i;
        //vUV.x = clamp(vUV.x, 0, 1);
        vUV.x = saturate(vUV.x);
		float4 currentColor = g_BackBufferTexture.Sample(g_LinearClampSampler, vUV);
		vColor += currentColor;
	}

	return vColor / 10.f;
}


float4 PS_FADE(PS_IN In) : SV_Target0
{
    PS_OUT Out = (PS_OUT) 0;

    float4 currentColor = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);	
    float4 vFinColor = currentColor * g_fFadeRate;

    return float4(vFinColor.xyz, 1.f);
}

float4 PS_BLACKOUT(PS_IN In) : SV_Target0
{
    return float4(0.f, 0.f, 0.f, 1.f);
}


float4 PS_NOISE(PS_IN In) : SV_Target0
{
    PS_OUT Out = (PS_OUT) 0;

    //float2 vUV = In.vTexcoord;
	
    //vUV.y = -1.f - (vUV.y);
    //vUV.x += sin(vUV.y * 10.f + g_fDeltaTime) / 10.f;
	
    //float4 vFinColor = g_BackBufferTexture.Sample(g_LinearClampSampler, vUV);
    
   // float2 uv = fragCoord.xy / iResolution.xy;
	
    float2 vFocusPosition = float2(1280.f / 2, 720.f / 2);
    vFocusPosition.x /= 1280.f;
    vFocusPosition.y /= 720.f;
    
    float2 vFocus = vFocusPosition - In.vTexcoord;
    float fLength = sqrt(vFocus.x * vFocus.x + vFocus.y * vFocus.y);
	
    float sinArg = fLength * 10.f - g_fDeltaTime * 10.f;
    float slope = cos(sinArg);
    float4 color = g_BackBufferTexture.Sample(g_LinearClampSampler, In.vTexcoord + vFocus * slope * 0.05);
		
	
    return float4(color.xyz, 1.f);
}


float4 PS_VIDEO(PS_IN In) : SV_Target0
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vColor = 1;
  
    if (In.vTexcoord.x < 0.5)
    {
        vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vColor = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);	
    }
        
	
    return float4(vColor.xyz, 1.f);
}


float4 PS_ZOOMBLURSPLIT(PS_IN In) : SV_Target0
{
    PS_OUT Out = (PS_OUT) 0;

	// g_vZoomPoint : -1 ~ 1 사이의 스크린 투영 좌표를 0~1의 texcoord로 변환한 것
	// In.vTexcoord : 현재 픽셀의 텍스처 좌표

    float2 vDiff = ((In.vTexcoord - g_vZoomPoint) * 0.5f);
    float2 vUV = In.vTexcoord;

    float4 vColor = vector(0.f, 0.f, 0.f, 0.f);

    for (uint i = 1; i <= 10; ++i)
    {
        vUV += vDiff * g_fZoomBlurPower * i;
        //vUV.x = clamp(vUV.x, 0, 1);
        vUV.x = saturate(vUV.x);
        if (In.vTexcoord.x < 0.5)
        {
            float4 currentColor = g_BackBufferTexture.Sample(g_LinearClampSampler, In.vTexcoord);
            vColor += currentColor;
        }
        else
        {
            float4 currentColor = g_BackBufferTexture.Sample(g_LinearClampSampler, vUV);
            vColor += currentColor;
        }
    }

    return vColor / 10.f;
}

technique11	DefaultTechnique
{
	pass Debug //0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Blur_X //1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_BLUR_X();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}
	pass Blur_Y //2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_BLUR_Y();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}
	pass Distortion //3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}

	pass Outline // 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
	}

    pass MotionBlur // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOTION_BLUR();
    }

	/*BackBuffer + Target Blend*/
    pass Blend // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND();
    }

    pass ChromaticAberration // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CHROMATIC_ABERRATION();
    }

	pass ZoomBlur // 8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ZOOM_BLUR();
	}


    pass Fade // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FADE();
    }

    pass BlackOut // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLACKOUT();
    }

    pass Noise // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NOISE();
    }

    pass Video // 12
    {

        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_VIDEO();

    }

    pass ZoomBlurSplit	// 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ZOOMBLURSPLIT();
    }
}

 