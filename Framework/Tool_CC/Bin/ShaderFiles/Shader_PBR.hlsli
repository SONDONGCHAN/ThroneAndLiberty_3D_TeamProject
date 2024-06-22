/*
https://blog.naver.com/fah204/221587912963
*BRDF : 양방향 반사분포함수 , 인자 두 개를 받으면 그 관계에 따라 반사율(반사광/입사광)을 출력하는 함수

실시간 렌더링 파이프라인에서는 Cook - Torrance BRDF 식을 사용
*Cook - Torrance BRDF : 왼쪽에는 Lambertian diffuse(diffuse Shading과 유사), 오른쪽은 Cook-Torrance Specular식이 있다
Cook - Torrance Specular BRDF는 3개의 함수로 이루어져있고, 각각 정규화 식이 있음

Trowbridge-Reitz GGX(D), Fresnel-Schlick(F), Smith's Schlick-GGX(G) -> 언리얼엔진에서 사용하는 라인업

*/


float fGamma = 2.2;
float PI = 3.14159;
float3 fFdielectric = 0.04;
float fEpsilon = 0.00001;


float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    
    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

//D : halfway vector h와 일치하는 표면 영역의 근사치를 구함
float NormalDistributionGGXTR(float3 n, float3 h, float a)
{
    float a2 = a * a;
    float NdotH = max(saturate(dot(n, h)), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.f) + 1.f);
    denom = PI * denom * denom;
    
    return nom / denom;
    
}

// G: 에너지 보존 법칙
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.f;
    float k = (r * r) / 8.f;
    
    float nom = NdotV;
    float denom = NdotV * (1.f - k) + k;
    
    return nom / denom;
}

float GeometrySmith(float3 n, float3 v, float3 l, float k)
{
    
    //Obstruction
    float NdotV = saturate(dot(n, v));
    
    //Shadowing
    float NdotL = saturate(dot(n, l));
    
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
  
    return ggx1 * ggx2;

}

float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);

}

float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
    
    
}


//F: 표면 각도에 따른 반사율 -> 우리가 보는 각도에 따른 반사 굴절
//재질이나 물질마다 빛이 반사되고 굴절되는 정도가 다름, 하지만 우리가 그걸 하나하나 값으로 넣어줄 수 없기때문에 수식을 이용해서 근사치를 구함
float3 FresnelSchlick(float fCosTheta, float3 R0)
{
    return (R0 + (1.f - R0) * pow(clamp(1.0 - fCosTheta, 0.0, 1.0), 5.0f));
}



