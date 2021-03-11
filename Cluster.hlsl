cbuffer CB : register(b0)
{
    float4x4 mView;
    float4x4 mInvView;
    float4x4 mProjection;
}

struct PointLight
{
    float4 mPosition;
    float3 mColor;
    float mIntensity;
    float mRadius;
};

StructuredBuffer<PointLight> gLights : register(t0);
Texture2D<float4> gWorldPosition : register(t1);

RWTexture3D<uint2> gClusterMap : register(u0);

groupshared uint x = 0;

static const float gNear = 0.01f;
static const float gFar = 1000.0f;
static const unsigned int gSubCount = 4;

[numthreads(16, 16, 4)]
void CSMain( uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint Gi : SV_GroupIndex)
{
    float3 viewPos = float3(mInvView._14, mInvView._24, mInvView._34);
    
    float near = gNear;
    float far = gFar;
    
    for (unsigned int i = 0; i < 4; i++)
    {
        float near = far - near;
    }
    
        gClusterMap[DTid.xyz] = uint2(Gi, 0);
    GroupMemoryBarrierWithGroupSync();
    
    
    
}