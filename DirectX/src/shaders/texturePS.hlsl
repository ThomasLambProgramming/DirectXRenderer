#define NUM_LIGHTS 4
//Sampler is for selecting pixels from the texture and it alls us to modify how pixels are written to the polygon face when shaded. (example from rastertek:
//For example, if the polygon is really far away and only makes up 8 pixels on the screen then we use the sample state to figure out which pixels or what combination of pixels will actually be drawn from the original texture.)
//There is Sampler2D which combines both but requires newer version of directx so wont be fully compatible

Texture2D ShaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer
{
    float4 diffuseColor[NUM_LIGHTS];
}

cbuffer PixelBuffer
{
    float4 pixelColor;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color: COLOR;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float3 directionToLight[NUM_LIGHTS] : TEXCOORD1;
};

float4 TextureSingleSamplePixelShader(PixelInputType a_Input) : SV_TARGET
{
    //sample the color from the texture using the sampler at this texture coordinate location.
    float4 textureColor = ShaderTexture.Sample(SampleType, a_Input.tex);
    return textureColor;
}

float4 TextureMultiLightPixelShader(PixelInputType a_Input) : SV_TARGET
{
    float lightIntensity[NUM_LIGHTS];
    float4 colorArray[NUM_LIGHTS];
    float4 colorSum = float4(0.0f,0.0f,0.0f,0.0f);
    float4 textureColor = ShaderTexture.Sample(SampleType, a_Input.tex);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        lightIntensity[i] = saturate(dot(a_Input.normal, a_Input.directionToLight[i]));
        colorArray[i] = diffuseColor[i] * lightIntensity[i];
    }

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        colorSum.r += colorArray[i].r;
        colorSum.g += colorArray[i].g;
        colorSum.b += colorArray[i].b;
    }
    float4 color = saturate(colorSum) * textureColor;
    return color;
}

float4 TextureFontSamplePixelShader(PixelInputType input) : SV_TARGET
{
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 textureColor = ShaderTexture.Sample(SampleType, input.tex);

    if (textureColor.r == 0.0f)
    {
        textureColor.a = 0.0f;
    }
    else
    {
        textureColor.a = 1.0f;
        textureColor = textureColor * pixelColor;
    }
    
    return textureColor;
}

float4 TextureRawColorPixelShader(PixelInputType input) : SV_TARGET
{
    return input.color;
}


