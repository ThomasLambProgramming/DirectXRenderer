Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
}
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 LightPixelShader(PixelInputType a_Input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    //Sample the pixel color from the texture.
    textureColor = shaderTexture.Sample(SampleType, a_Input.tex);

    //inverse (normal dot -light = if both in same direction (dot=1) then we give full lighting.
    lightDir = -lightDirection;
    //saturate= clamp(0,1).
    lightIntensity = saturate(dot(a_Input.normal, lightDir));
    color = saturate(diffuseColor * lightIntensity);
    color = color * textureColor;
    return color;
}