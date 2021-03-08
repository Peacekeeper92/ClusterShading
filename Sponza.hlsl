cbuffer CB
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProjection;
};

struct a2v
{
    float4 mPosition : POSITION0;
    float3 mNormal : NORMAL0;
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

    
    output.mNormal = float4(mul(input.mNormal, (float3x3) gWorld), 1.0f);
    
    output.mTexcoord = input.mTexcoord;

    return output;
}

float4 PSMain(v2p input) : SV_Target0
{
    return normalize(input.mPosition);
}

