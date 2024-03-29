#define NUM_LIGHTS 4

//General purpose texture shader.
//Can take in lights and blend multiple textures.
//Mainly used for testing multiple different texture stuff so I do not
//have to rewrite the shaders / buffer information for testing something new
//Amazing that light sampling, multi texture, color pixel shaders all use almost the same vertex shader.

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}
cbuffer CameraBuffer 
{
    float3 cameraPosition;
    float cameraPadding;
}

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex: TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal: NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
};


PixelInputType TextureVertexShader(VertexInputType a_input)
{
    PixelInputType output;

    //Matrix calculations require w to be 1. 
    a_input.position.w = 1.0f;

    //Place object in correct position for rendering using world, view and projection matrices.
    output.position = mul(a_input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = a_input.tex;

    output.normal = normalize(mul(a_input.normal, (float3x3)worldMatrix));
    output.tangent = normalize(mul(a_input.tangent, (float3x3)worldMatrix));
    output.binormal = normalize(mul(a_input.binormal, (float3x3)worldMatrix));

    float4 worldPosition = mul(a_input.position, worldMatrix);
    output.viewDirection = normalize(cameraPosition.xyz - worldPosition.xyz);
    return output;
}
