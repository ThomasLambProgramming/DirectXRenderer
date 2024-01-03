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

cbuffer LightInformationBuffer
{
    float4 lightPosition[NUM_LIGHTS];
    float4 LightColor[NUM_LIGHTS];
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 tex: TEXCOORD0;
    float2 blendTex1 : TEXCOORD1;
    float2 blendTex2 : TEXCOORD2;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal: NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex : TEXCOORD0;
    float2 blendTex1 : TEXCOORD1;
    float2 blendTex2 : TEXCOORD2;
    float3 directionToLight[NUM_LIGHTS] : TEXCOORD3;
};


PixelInputType TextureVertexShader(VertexInputType a_input)
{
    PixelInputType output;

    a_input.position.w = 1.0f;

    output.position = mul(a_input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = a_input.tex;
    output.blendTex1 = a_input.blendTex1;
    output.blendTex2 = a_input.blendTex2;

    output.color = a_input.color;

    output.normal = normalize(mul(a_input.normal, worldMatrix));


    float4 worldPosition = mul(a_input.position, worldMatrix);
    for (int i = 0; i < NUM_LIGHTS; i++)
        output.directionToLight[i] = normalize(lightPosition[i] - worldPosition.xyz);

    return output;
}
