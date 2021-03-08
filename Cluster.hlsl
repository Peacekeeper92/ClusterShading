RWTexture3D<uint2> gClusterMap : register(u0);

static const float4 gPoint1 = float4(0.0f, 1.5f, 1.0f, 1.0f);


groupshared uint x = 0;


[numthreads(16, 16, 4)]
void CSMain( uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint Gi : SV_GroupIndex)
{
    gClusterMap[DTid.xyz] = uint2(Gi, 0);
    GroupMemoryBarrierWithGroupSync();
    
    
    //if(Gi == 0)
    //{
    //    x = 0;

    //}
    //    GroupMemoryBarrierWithGroupSync();
    
    //if (Gi == 1)
    //{
    //    x = 1;

    //}
    //GroupMemoryBarrierWithGroupSync();
    //if (Gi == 2)
    //{
    //    x = 2;

    //}
    //GroupMemoryBarrierWithGroupSync();
    //if (Gi == 3)
    //{
    //    x++;

    //}
    //GroupMemoryBarrierWithGroupSync();
    //if (Gi == 4)
    //{
    //    x++;

    //}
    //GroupMemoryBarrierWithGroupSync();
    
    
    
}