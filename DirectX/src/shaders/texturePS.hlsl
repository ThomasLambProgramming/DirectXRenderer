#define NUM_LIGHTS 4
Texture2D ShaderTexture1 : register(t0);
Texture2D ShaderTexture2 : register(t1);
Texture2D ShaderTexture3 : register(t2);
SamplerState Sampler : register(s0);

cbuffer LightInformationBuffer
{
    float4 lightPosition[NUM_LIGHTS];
    float4 lightDiffuse[NUM_LIGHTS];
    float4 lightSpecularColor[NUM_LIGHTS];
    float3 lightDirection[NUM_LIGHTS];
    float lightSpecularPower[NUM_LIGHTS];
    float4 lightAmbient[NUM_LIGHTS];
}
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal: NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
};

float4 SpecularMapPixelShader(PixelInputType input) : SV_TARGET
{
    //This first section is the same as the normal map above.
    float4 textureColor = ShaderTexture1.Sample(Sampler, input.tex);
    float4 normalMap = ShaderTexture2.Sample(Sampler, input.tex);
    normalMap = (normalMap * 2.0f) - 1.0f;
    float3 bumpNormal = normalize((normalMap.x * input.tangent) + (normalMap.y * input.binormal) + (normalMap.z * input.normal));
    float lightIntensity = saturate(dot(bumpNormal, -lightDirection[0]));
    float4 color = saturate(lightDiffuse[0] * lightIntensity) * textureColor;

    if (lightIntensity <= 0.0f)
        return color;

    float4 specularIntensity = ShaderTexture3.Sample(Sampler, input.tex);

    //Make sure to use the normal maps so specular isnt just going off a flat surface.
    //reflection is the direction.
    float3 reflection = normalize(2 * lightIntensity * bumpNormal + lightDirection[0]);
    //limit the dot of the reflection direction and the view direction and then multiply by the preset specular power amount)
    float4 specular = pow(saturate(dot(reflection, input.viewDirection)), lightSpecularPower[0]) * specularIntensity;
    color = saturate(color + specular);

    return color;
}
 

