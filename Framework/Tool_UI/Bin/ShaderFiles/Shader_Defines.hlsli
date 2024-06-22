/* For.Sampler_State */

sampler g_LinearSampler = sampler_state
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
    MaxLod = 3.402823466e+38f;
};

sampler g_LinearMirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;
};

sampler g_LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

sampler g_LinearBorderSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

sampler g_PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler g_ShadowSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
    AddressW = clamp;
};

SamplerComparisonState g_ShadowCompSampler = sampler_state
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    ComparisonFunc = LESS_EQUAL;
};

sampler AnisotropicClampSampler = sampler_state
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;
    AddressU = clamp;
    AddressV = clamp;
    AddressW = clamp;
    ComparisonFunc = ALWAYS;
    MaxLOD = 0.f;
    MinLOD = 0.f;
};


RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = true;
};

RasterizerState RS_WireFrame
{
    FillMode = WireFrame;
    CullMode = None;
};


DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};


DepthStencilState DSS_ORTHO
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_Greater
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None_ZTestAndWrite
{
    DepthEnable = false;
    DepthWriteMask = zero;

};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlending
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_Blending
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

uint g_iWeights = 19;
float g_fWeight[19] =
{
    0.0561f, 0.1353f, 0.278f,
    0.3221f, 0.4868f, 0.5881f,
    0.6661f, 0.7261f, 0.9231f,
    0.999f,
    0.9231f, 0.7261f, 0.6661f,
    0.5881f, 0.4868f, 0.3221f,
    0.278f, 0.1353f, 0.0561f
};

uint g_iLowWeights = 11;
float g_fLowWeight[11] =
{
    0.1353f, 0.278f, 0.4868f, 
    0.7261f, 0.9231f, 
    1.0f, 
    0.9231f, 0.7261f, 0.4868f, 
    0.278f, 0.1353f
};

float		g_fTexW = 1280.0f;
float		g_fTexH = 720.0f;


float4	Blur_X(float2 vTexcoord, texture2D g_BlurTexture)
{
    float4		vOut = (float4)0;

    float2		vUV = (float2)0;
    float		fTotal = 0.f;

    int iValue = (g_iLowWeights - 1);

    for (int i = -iValue; i < iValue; ++i)
    {
        vUV = vTexcoord + float2((1.f / g_fTexW) * i, 0.f);

        float weight = g_fLowWeight[iValue + i];
        vOut += weight * g_BlurTexture.Sample(g_LinearSampler, vUV);
        fTotal += weight;
    }

    vOut /= fTotal;

    return vOut;
}

float4	Blur_Y(float2 vTexcoord, texture2D g_BlurTexture)
{
    float4		vOut = (float4)0;

    float2		vUV = (float2)0;
    float		fTotal = 0.f;

    int iValue = (g_iLowWeights - 1);

    for (int i = -iValue; i < iValue; ++i)
    {
        vUV = vTexcoord + float2(0.f, (1.f / g_fTexH) * i);

        float weight = g_fLowWeight[iValue + i];
        vOut += weight * g_BlurTexture.Sample(g_LinearSampler, vUV);
        fTotal += weight;
    }

    vOut /= fTotal;

    return vOut;
}