cbuffer CB : register(b0)
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProjection;
};

struct PointLight
{
    float4 mPosition;
    float3 mColor;
    float mIntensity;
    float mRadius;
    
};

SamplerState gDefaultSampler : register(s0);

StructuredBuffer<PointLight> gLights : register(t0);
Texture2D<float4> gPosition : register(t1);
Texture2D<float4> gNormal : register(t2);
Texture2D<float2> gTexcoord : register(t3);
Texture2D<float4> gWorldPosition : register(t4);


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
    float4 mWorldPosition : POSITION0;
};

struct ga2v
{
    float4 mPosition : POSITION0;
    float2 mTexcoord : TEXCOORD0;
};

struct gv2p
{
    float4 mPosition : SV_Position;
    float2 mTexcoord : TEXCOORD0;
};

struct gp2b
{
    float4 mPosition : SV_Target0;
    float4 mNormal : SV_Target1;
    float2 mTexcoord : SV_Target2;
    float4 mWorldPosition : SV_Target3;

};



v2p VSMain(a2v input)
{
    v2p output = (v2p) 0;
    input.mPosition.w = 1.0f;
    output.mPosition = mul(input.mPosition, gWorld);
    output.mWorldPosition = output.mPosition;
    output.mPosition = mul(output.mPosition, gView);
    output.mPosition = mul(output.mPosition, gProjection);

    
    output.mNormal = float4(mul(input.mNormal, (float3x3) gWorld), 0.0f);
    output.mTexcoord = input.mTexcoord;

    return output;
}



gp2b PSMain(v2p input)
{
    gp2b output = (gp2b)0;

    output.mPosition = input.mPosition;
    output.mNormal = input.mNormal;
    output.mTexcoord = input.mTexcoord;
    output.mWorldPosition = input.mWorldPosition;
    
    return output;
}

gv2p QuadVSMain(ga2v input)
{
    gv2p output = (gv2p) 0;
    
    output.mPosition = input.mPosition;
    output.mTexcoord = input.mTexcoord;
    
    return output;
}

float4 QuadPSMain(gv2p input) : SV_Target0
{
    float4 position = gPosition.Sample(gDefaultSampler, input.mTexcoord);
    float4 normal = gNormal.Sample(gDefaultSampler, input.mTexcoord);
    float2 uv = gTexcoord.Sample(gDefaultSampler, input.mTexcoord);
    float4 worldPos = gWorldPosition.Sample(gDefaultSampler, input.mTexcoord);
    
    float4 lightDir = gLights[0].mPosition - worldPos;
    float atten = distance(worldPos, gLights[0].mPosition);
    float intensity = gLights[0].mIntensity;
    
    float dist = length(lightDir);
    
    lightDir /= dist;

    float diffuse = dot(lightDir, normal) * intensity / atten;
    float3 color = diffuse * gLights[0].mColor;
    
    return float4(color, 1.0f);
}