/*림라이트 color 계산*/
/*_vRimCol = 림라이트 색, g_fRimPow은 높으면 높을수록 영역이 좁아짐*/
float3 ComputeRimColor(float3 _vRimCol , float3 _vLook, float3 _vNormal, float fRimPow)
{
    float3 E = normalize(_vLook);
    
    float value = saturate(dot(_vNormal, E));
    float fEmissive = 1.0f - value;

    /*min 숫자가 높으면 높을수록 색이 옅어짐*/
    fEmissive = smoothstep(0.f, 1.0f, fEmissive);

    /*숫자가 높으면 높을수록 영역이 좁아짐*/ 
    fEmissive = pow(fEmissive, fRimPow);
    float3 vRimFinColor = fEmissive * _vRimCol;

    return vRimFinColor;
}

float4 Dissolve(float _fNoiseR, float _fStart, float _fEnd, float _fThickness, float4 _vDslvColor, float4 _vColor)
{
    float4 vColor = _vColor;

    float fNoiseR = _fNoiseR * _fNoiseR;

    if (fNoiseR < _fStart)
    {
        // 그대로 출력
    }
    else if (fNoiseR < _fEnd)
    {
        float fValue = smoothstep(0.0f, 1.0f, (_fEnd - fNoiseR) * _fThickness);

        vColor.rgb += lerp(_vColor.rgb, _vDslvColor.rgb, fValue);
    }
    else
    {
        discard;
    }

    return vColor;
}