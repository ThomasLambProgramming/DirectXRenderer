#define NUM_LIGHTS 4

//General purpose texture shader.
//Can take in lights and blend multiple textures.
//Mainly used for testing multiple different texture stuff so I do not
//have to rewrite the shaders / buffer information for testing something new

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

cbuffer LightInformationBuffer
{
    float4 lightPosition[NUM_LIGHTS];
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : Color;
    float2 tex: TEXCOORD0;
    float2 blendTex1 : TEXCOORD1;
    float2 blendTex2 : TEXCOORD2;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal: NORMAL;
    float2 tex : TEXCOORD0;
    float2 blendTex1 : TEXCOORD1;
    float2 blendTex2 : TEXCOORD2;
    float3 directionToLight[NUM_LIGHTS] : TEXCOORD3;
};


PixelInputType TextureVertexShader(VertexInputType a_Input)
{
    PixelInputType output;

    a_Input.position.w = 1.0f;

    output.position = mul(a_Input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = a_Input.tex;
    output.blendTex1 = a_Input.blendTex1;
    output.blendTex2 = a_Input.blendTex2;

    output.color = a_Input.color;

    output.normal = normalize(mul(a_Input.normal, worldMatrix));


    float4 worldPosition = mul(a_Input.position, worldMatrix);
    for (int i = 0; i < NUM_LIGHTS; i++)
        output.directionToLight[i] = normalize(lightPosition[i] - worldPosition.xyz);

    return output;
}
