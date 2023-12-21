#define NUM_LIGHTS 4

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor[NUM_LIGHTS];
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
    float3 lightPos[NUM_LIGHTS] : TEXCOORD2;
};

float4 LightPixelShader(PixelInputType a_Input) : SV_TARGET
{
    float4 textureColor;
    float4 color = ambientColor;
    float3 reflection;
    float4 specular;
    float4 colorSum;
    float lightIntensity[NUM_LIGHTS];
    float4 colorArray[NUM_LIGHTS];
    int i; 
    specular = float4(0.0f,0.0f,0.0f,0.0f);
    colorSum = float4(0.0f,0.0f,0.0f,0.0f);
    //Sample the pixel color from the texture.
    textureColor = shaderTexture.Sample(SampleType, a_Input.tex);

    for (i = 0; i < NUM_LIGHTS; i++)
    {
        //lightpos = direction to light from vertex
        lightIntensity[i] = saturate(dot(a_Input.normal, a_Input.lightPos[i]));
        colorArray[i] = diffuseColor[i] * lightIntensity[i];
        colorSum.rgb += colorArray[i].rgb;
        
        //Shader book and tutorial giving different ways of shading but whatever decide on that later.
        //float3 halfwayVector = normalize(lightDirection + viewDirection);
        //return colorReflection * specularIntensity * pow(max(0,dot(normal, halfwayVector)), specularPower);

        reflection = normalize(2.0f * lightIntensity[i] * a_Input.normal - a_Input.lightPos[i]);
        specular += pow(saturate(dot(reflection, a_Input.viewDirection)), specularPower);
    }

    color = saturate(saturate(colorSum) * textureColor * specular);
    color = saturate(color * ambientColor);
    color = float4(1.0f,1.0f,1.0f,1.0f);
    return color;
}