
namespace s3d
{
    //
    //	VS Input
    //
    struct VSInput
    {
        float4 position : POSITION;
        float3 normal : NORMAL;
        float2 uv : TEXCOORD0;
        uint vertexID : SV_VertexID;
    };

    //
    //	VS Output / PS Input
    //
    struct PSInput
    {
        float4 position : SV_POSITION;
        float3 worldPosition : TEXCOORD0;
        float2 uv : TEXCOORD1;
        float3 normal : TEXCOORD2;
    };
}

//
//	Constant Buffer
//
cbuffer VSPerView : register(b1)
{
    row_major float4x4 g_worldToProjected;
}

cbuffer VSPerObject : register(b2)
{
    row_major float4x4 g_localToWorld;
}

cbuffer VSPerMaterial : register(b3)
{
    float4 g_uvTransform;
}

cbuffer CBVSBoneData : register(b4)
{
    row_major float4x4 g_boneMatrices[256];
};

Texture2D g_boneIndices : register(t0);
Texture2D g_boneWeights : register(t1);

//
//	Functions
//
s3d::PSInput VS(
    s3d::VSInput input
)
{
    s3d::PSInput result;
    
    // 頂点IDから2Dテクスチャ上の座標(X, Y)を逆算する（幅1024）
    int texX = input.vertexID % 1024;
    int texY = input.vertexID / 1024;
    int3 texBoneIndex = int3(texX, texY, 0);
    
    float4 indices = g_boneIndices.Load(texBoneIndex);
    float4 weights = g_boneWeights.Load(texBoneIndex);
    
    float4x4 skinMatrix =
        mul(weights.x, g_boneMatrices[(int) indices.x])
        + mul(weights.y, g_boneMatrices[(int) indices.y])
        + mul(weights.z, g_boneMatrices[(int) indices.z])
        + mul(weights.w, g_boneMatrices[(int) indices.w]);
    
    float4 localPos = mul(float4(input.position.xyz, 1.0f), skinMatrix);
    float3 localNormal = mul(input.normal, (float3x3)skinMatrix);
    
    float4 worldPos = mul(localPos, g_localToWorld);

    result.position = mul(worldPos, g_worldToProjected);
    result.worldPosition = worldPos.xyz;
    result.uv = (input.uv * g_uvTransform.xy + g_uvTransform.zw);
    result.normal = normalize(mul(localNormal, (float3x3)g_localToWorld));
    return result;
}