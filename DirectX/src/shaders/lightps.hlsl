#define NUM_LIGHTS 4

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer
{
    float4 diffuseColor[NUM_LIGHTS];
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 lightPos[NUM_LIGHTS] : TEXCOORD2;
};

float4 LightPixelShader(PixelInputType a_Input) : SV_TARGET
{
    float4 textureColor;
    float4 color = float4(0.0f,0.0f,0.0f,1.0f);
    float4 colorSum;
    float lightIntensity[NUM_LIGHTS];
    float4 colorArray[NUM_LIGHTS];
    int i; 
    colorSum = float4(0.0f,0.0f,0.0f,0.0f);
    //Sample the pixel color from the texture.
    textureColor = shaderTexture.Sample(SampleType, a_Input.tex);

    for (i = 0; i < NUM_LIGHTS; i++)
    {
        lightIntensity[i] = saturate(dot(a_Input.normal, a_Input.lightPos[i]));
        colorArray[i] = diffuseColor[i] * lightIntensity[i];
    }

    for (i = 0; i < NUM_LIGHTS; i++)
    {
        colorSum.r += colorArray[i].r;
        colorSum.g += colorArray[i].g;
        colorSum.b += colorArray[i].b;
    }
    color = saturate(colorSum) * textureColor;
    return color;
}