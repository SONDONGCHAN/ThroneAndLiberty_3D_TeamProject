#include "Shader_Defines.hlsli"
#include "Shader_PBR.hlsli"

matrix	g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix  g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;


texture2D g_Texture;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vLightPos;
float g_fLightRange;

vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;
vector g_vCamLook;

Texture2D g_NormalTexture;
Texture2D g_DiffuseTexture;
Texture2D g_ShadeTexture;
Texture2D g_DepthTexture;
Texture2D g_SpecularTexture;
Texture2D g_EmissiveTexture;
Texture2D g_ARMTexture;
Texture2D g_AOTexture;
Texture2D g_ShadowTexture;
Texture2D g_FinalShadowTexture;
Texture2D g_RimTexture;
Texture2D g_NoiseTexture;

float g_fNoiseSpeed;

//Texture2D g_CascadeTexture1;
Texture2D g_CascadeTexture2;
Texture2D g_CascadeTexture3;

Texture2DArray g_CascadeTexture1;
/*Cascade Shadow*/
matrix g_CascadeProjMat[3];
matrix g_CascadeViewMat[3];

Texture2DArray g_CascadeTextures;
SamplerComparisonState samplerComp;

float g_CascadeEndClip[3] = { 20.f, 35.f, 50.f };

float g_fFar = 1000.f;

bool g_IsUsingNoise = false;
bool g_IsUsingFog = false;

/*IBL*/
TextureCube g_irradianceCube;

float g_fScreenWidth;
float g_fScreenHeight;


/*Debug Option*/
bool g_IsPBROn;
bool g_IsAOOn;

/*AO*/

float g_fAOPower = 7.f;

/*Fog*/
float g_fFogStart;
float g_fFogEnd;
float g_fFogHeight;
float g_fFogStartHeight;

float g_fFogChangeSpeed;
float g_fFogDensity;
float g_fFogMinValue;
float3 g_vFogColor;

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

    Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
    vector vFinalShadow : SV_TARGET2;
};

struct PS_OUT_LIGHT2
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};


PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    //https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
    //https://blog.naver.com/PostView.nhn?blogId=oovqiss&logNo=221666234384&parentCategoryNo=&categoryNo=11&viewDate=&isShowPopularPosts=true&from=search
    
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDiffuseDesc = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(g_PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);
    vector vARMDesc = g_ARMTexture.Sample(g_LinearSampler, In.vTexcoord);
           
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    /*ARM Texture 각 요소 */
    float AO = vARMDesc.r;
    float Roughness = vARMDesc.g;    
    float metallic = vARMDesc.b;
   
    float fHalfLambert = dot(normalize(g_vLightDir) * -1.f, vNormal) * 0.5f + 0.5f;
    
    Out.vShade = g_vLightDiffuse *
		saturate(saturate(fHalfLambert) +
		(g_vLightAmbient * g_vMtrlAmbient));
    
    vector vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));
    
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    float fViewZ = vDepthDesc.y * g_fFar;
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
      /*Cascade Shadow*/
   
    float fShadowPower = 1.f;

    float vLightDepth= 0.f;
    
    if (fViewZ <= 20.f)
    {
    
        float2 vUV;

        float4 vLightViewPos = mul(vWorldPos, g_CascadeViewMat[0]);
        float4 vLightProjPos = mul(vLightViewPos, g_CascadeProjMat[0]);
    
        vLightProjPos = vLightProjPos / vLightProjPos.w;
    
        vUV.x = vLightProjPos.x * 0.5f + 0.5f;
        vUV.y = vLightProjPos.y * -0.5f + 0.5f;
    
        float fNormalOffset = 0.00011282f;
        float fDot = saturate(dot(normalize(g_vLightDir.xyz) * -1.f, vNormalDesc.xyz));
        float fBias = max((fNormalOffset * 5.0f) * (1.0f - (fDot * -1.0f)), fNormalOffset);
        
        if (vLightProjPos.z > 1.0 || vLightProjPos.z < 0.0)
        {
            fShadowPower = 1.f;

        }
        else
        {
        
            [unroll]
            for (int x = -1; x <= 1; ++x)
            {
                [unroll]
                for (int y = -1; y <= 1; ++y)
                {
                    vector vLightDepth = g_CascadeTexture1.SampleCmpLevelZero(g_ShadowCompSampler, float3(vUV, 0), vLightProjPos.z, int2(x, y));
            
            
                    if (vLightProjPos.z - fBias > vLightDepth.x)
                    {
                        fShadowPower += 0.3f;
                    }
                    else
                    {
                        fShadowPower += 1.f;

                    }
                }
            }
    
                fShadowPower /= 9.0f;
            }
    }
    else if (fViewZ > 20.f && fViewZ <= 35.f)
    {
    
        float2 vUV;

        float4 vLightViewPos = mul(vWorldPos, g_CascadeViewMat[1]);
        float4 vLightProjPos = mul(vLightViewPos, g_CascadeProjMat[1]);
    
        vLightProjPos = vLightProjPos / vLightProjPos.w;
    
        vUV.x = vLightProjPos.x * 0.5f + 0.5f;
        vUV.y = vLightProjPos.y * -0.5f + 0.5f;
    
        float fNormalOffset = 0.00011282f;
        float fDot = saturate(dot(normalize(g_vLightDir.xyz) * -1.f, vNormalDesc.xyz));
        float fBias = max((fNormalOffset * 5.0f) * (1.0f - (fDot * -1.0f)), fNormalOffset);
        
        if (vLightProjPos.z > 1.0 || vLightProjPos.z < 0.0)
        {
            fShadowPower = 1.f;

        }
        else
        {
        
            [unroll]
            for (int x = -1; x <= 1; ++x)
            {
                [unroll]
                for (int y = -1; y <= 1; ++y)
                {
                    vector vLightDepth = g_CascadeTexture1.SampleCmpLevelZero(g_ShadowCompSampler, float3(vUV, 1), vLightProjPos.z, int2(x, y));
            
                    if (vLightProjPos.z - fBias > vLightDepth.x)
                    {
                        fShadowPower += 0.3f;
                    }
                    else
                    {
                        fShadowPower += 1.f;

                    }
                    
                    
                }
            }
    
            fShadowPower /= 9.0f;
        }
    }
    else if (fViewZ > 35.f && fViewZ <= 300.f)
    {
    
        float2 vUV;

        float4 vLightViewPos = mul(vWorldPos, g_CascadeViewMat[2]);
        float4 vLightProjPos = mul(vLightViewPos, g_CascadeProjMat[2]);
    
        vLightProjPos = vLightProjPos / vLightProjPos.w;
    
        vUV.x = vLightProjPos.x * 0.5f + 0.5f;
        vUV.y = vLightProjPos.y * -0.5f + 0.5f;
    
        float fNormalOffset = 0.00011282f;
        float fDot = saturate(dot(normalize(g_vLightDir.xyz) * -1.f, vNormalDesc.xyz));
        float fBias = max((fNormalOffset * 5.0f) * (1.0f - (fDot * -1.0f)), fNormalOffset);
        
        
        if (vLightProjPos.z > 1.0 || vLightProjPos.z < 0.0)
        {
            fShadowPower = 1.f;

        }
        else
        {
        
            [unroll]
            for (int x = -1; x <= 1; ++x)
            {
                [unroll]
                for (int y = -1; y <= 1; ++y)
                {
                    vector vLightDepth = g_CascadeTexture1.SampleCmpLevelZero(g_ShadowCompSampler, float3(vUV, 2), vLightProjPos.z, int2(x, y));
            
                    if (vLightProjPos.z - fBias > vLightDepth.x)
                    {
                        fShadowPower += 0.3f;
                    }
                    else
                    {
                        fShadowPower += 1.f;

                    }
                }
            }
    
            fShadowPower /= 9.0f;
        }
    }
    
       
    Out.vFinalShadow = fShadowPower;
    
    
    /*빛연산이 더하기 연산이기 때문에 이상한 곳에 더해지지 않게 깊이값을 비교해서 다르면 연산하지 않음*/
    if (vDepthDesc.x != vDepthDesc.z)
    {
        return Out;
    }
    
   
    vector vLook = vWorldPos - g_vCamPosition;
    vector vLookInv = vLook;
    //vector vLookInv = g_vCamPosition - vWorldPos;
   
    float3 albedo = pow(vDiffuseDesc.xyz, fGamma);
    
    if ((0.f == AO && 0.f == Roughness && 0.f == metallic) || !g_IsPBROn)
    {
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        float3 N = vNormal.xyz;
        float3 Lo = (normalize(g_vCamPosition - vWorldPos)).xyz;
    
        float fHalfLambert = dot(N, Lo) * 0.5f + 0.5f;
        float cosLo = max(0.0, fHalfLambert);
        float3 Lr = 2.0 * cosLo * N - Lo;
    
    
        float3 F0 = lerp(fFdielectric, albedo, metallic);
        float3 radiance = g_vLightDiffuse.xyz;
    
        float3 Li = -g_vLightDir.xyz;
    
        float3 Lh = normalize(Li + Lo);
        float cosLi = max(0.0, dot(N, Li));
        float cosLh = max(0.0, dot(N, Lh));
    
    //Fresnel
        float3 F = FresnelSchlick(max(0.0, dot(Lh, Lo)), F0);
    //Diffuse
        float D = ndfGGX(cosLh, Roughness);
    
        float G = gaSchlickGGX(cosLi, cosLo, Roughness);

   
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
        float3 diffuseBRDF = kd * albedo;
        float3 SpecularBRDF = (F * D * G) / max(fEpsilon, 4.0 * cosLi * cosLo);
        
        if (!g_IsAOOn)
        {
            AO = 1.f;

        }
    
        float3 vAmbientLight = float3(0.f, 0.f, 0.f);
        AO = pow(AO, g_fAOPower);
    
        Out.vSpecular = float4((((diffuseBRDF + SpecularBRDF) * cosLi * radiance) * AO), 1.f);     

    }
       
    return Out;
}

PS_OUT_LIGHT2 PS_MAIN_POINT(PS_IN In)
{    
    PS_OUT_LIGHT2 Out = (PS_OUT_LIGHT2) 0;
    
    vector vDiffuseDesc = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(g_PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);
    vector vARMDesc = g_ARMTexture.Sample(g_LinearSampler, In.vTexcoord);
         
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    float vNoise = 1.f;
    
    if (g_IsUsingNoise)
    {
        vNoise = 1.f - (g_NoiseTexture.Sample(g_LinearSampler, In.vTexcoord + g_fNoiseSpeed).r);
        
    }
    
    /*ARM Texture 각 요소 */
    float AO = vARMDesc.r;
    float Roughness = vARMDesc.g;
    float metallic = vARMDesc.b;
   
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    float fViewZ = vDepthDesc.y * g_fFar;
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    /*빛연산이 더하기 연산이기 때문에 이상한 곳에 더해지지 않게 깊이값을 비교해서 다르면 연산하지 않음*/
    if (vDepthDesc.x != vDepthDesc.z)
    {
        return Out;
    }
    
   
    vector vLook = vWorldPos - g_vCamPosition;
    vector vLookInv = vLook;
   
    float3 albedo = pow(vDiffuseDesc.xyz, fGamma);
    
    if ((0.f == AO && 0.f == Roughness && 0.f == metallic) || !g_IsPBROn)
    {
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        vector vLightDir = vWorldPos - g_vLightPos;
        float fDistance = length(vLightDir);
		
	/* 빛의 감쇠값을 설정한다. */
        float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
            
        float3 N = vNormal.xyz;
        float3 Lo = (normalize(g_vCamPosition - vWorldPos)).xyz;
    
        float cosLo = max(0.0, dot(N, Lo));
        float3 Lr = 2.0 * cosLo * N - Lo;
    
    
        float3 F0 = lerp(fFdielectric, albedo, metallic);
        float3 radiance = g_vLightDiffuse.xyz;
    

        float3 Li = -vLightDir.xyz;
    
        float3 Lh = normalize(Li + Lo);
        float cosLi = max(0.0, dot(N, Li));
        float cosLh = max(0.0, dot(N, Lh));
    
        float3 F = FresnelSchlick(max(0.0, dot(Lh, Lo)), F0);
        float D = ndfGGX(cosLh, Roughness);
        float G = gaSchlickGGX(cosLi, cosLo, Roughness);

   
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
        float3 diffuseBRDF = kd * albedo;
        float3 SpecularBRDF = (F * D * G) / max(fEpsilon, 4.0 * cosLi * cosLo);
       
        Out.vSpecular += (float4((((diffuseBRDF + SpecularBRDF) * cosLi * radiance * fAtt)) * vNoise, 1.f));

    }
       
    return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    /*감마 공간에 있던 Diffuse를 선형공간으로 변환한 후 선형공간에서 계산 */
    /*모든 연산은 선형공간에서 이루어져야함*/
    vector vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
   // vDiffuse = pow(vDiffuse, 2.2f);

    if (0.f == vDiffuse.a)
        discard;
	
    vector vShade = g_ShadeTexture.Sample(g_LinearSampler, In.vTexcoord);

   // vector vEmissive = g_EmissiveTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    vector vFinalShadow = g_FinalShadowTexture.Sample(g_LinearSampler, In.vTexcoord);
    //vFinalShadow = vFinalShadow * 0.5f + 0.5f;
    
    vector vDepthDesc = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);
    vector vRimLight = g_RimTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vWorldPos;
    vector vColor = (vDiffuse * vShade + vSpecular + vRimLight) * vFinalShadow;
    
    //Out.vColor = vDiffuse;
    
    if (!g_IsUsingFog)
    {
        Out.vColor = float4(vColor.xyz, 1.f);

    }
    //Using Fog
    else
    {
        float height = vWorldPos.y;
        float fogFactor = 0.0f;

        float fCurrentDepth = vDepthDesc.y * 1000.f;

        fogFactor = saturate(fCurrentDepth / (g_fFogEnd - g_fFogStart));

        fogFactor = fogFactor * 0.7f;
        
        float3 finalColorWithFog = lerp(vColor.rgb, g_vFogColor, fogFactor);
        Out.vColor = float4(finalColorWithFog, 1.f);
    }
    

    return Out;

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
        ComputeShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Blending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Blending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }


    pass Deferred_Final //3 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

}


