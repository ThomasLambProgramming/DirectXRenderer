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
    float padding;
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

    // Change the position vector to be 4 units for proper matrix calculations.
    a_input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(a_input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = a_input.tex;

    // Calculate the normal vector against the world matrix only and then normalize the final value.
    output.normal = mul(a_input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);
    //Both the a_input tangent vector and binormal vector are calculated against the world matrix and then normalized the same way the input normal vector is.

    // Calculate the tangent vector against the world matrix only and then normalize the final value.
    output.tangent = mul(a_input.tangent, (float3x3)worldMatrix);
    output.tangent = normalize(output.tangent);

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
    output.binormal = mul(a_input.binormal, (float3x3)worldMatrix);
    output.binormal = normalize(output.binormal);

    output.viewDirection = normalize(cameraPosition.xyz - output.position.xyz);
    
    return output;
}
