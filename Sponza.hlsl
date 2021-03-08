cbuffer CB : register(b0)
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProjection;
};

struct PointLight
{
    float4 mPosition;
    float mIntensity;
    float3 mColor;
};

static const float4 gLightDir = float4(-0.5, 1, 0.5, 0.0f);
static const float4 gPoint1 = float4(0.0f, 1.5f, 1.0f, 1.0f);
static const float gPoint1Power = 2.0f;

struct a2v
{
    float4 mPosition : POSITION0;
    float3 mNormal : NORMAL;
    float2 mTexcoord : TEXCOORD0;
};

struct v2p
{
    float4 mPosition : SV_Position;
    float4 mNormal : TEXCOORD0;
    float2 mTexcoord : TEXCOORD1;
};

v2p VSMain(a2v input)
{
    v2p output = (v2p) 0;
    input.mPosition.w = 1.0f;
    output.mPosition = mul(input.mPosition, gWorld);
    output.mPosition = mul(output.mPosition, gView);
    output.mPosition = mul(output.mPosition, gProjection);

    
    output.mNormal = float4(mul(input.mNormal, (float3x3) gWorld), 0.0f);
    output.mTexcoord = input.mTexcoord;

    return output;
}

float4 PSMain(v2p input) : SV_Target0
{
    float4 lightDir = normalize(input.mPosition - gPoint1);
    
    float dist = distance(input.mNormal, gPoint1);
    
    return dot(abs(input.mNormal), lightDir) * gPoint1Power / dist;
    
    return dot(-input.mNormal, normalize(gLightDir));
}

