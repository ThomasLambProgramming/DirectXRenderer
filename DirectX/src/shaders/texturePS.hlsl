#define NUM_LIGHTS 4
Texture2D ShaderTexture1 : register(t0);
Texture2D ShaderTexture2 : register(t1);
Texture2D ShaderTexture3 : register(t2);

Texture2D ReflectionTexture : register(t3);
Texture2D RefractionTexture : register(t4);
SamplerState Sampler : register(s0);

cbuffer LightInformationBuffer
{
    //Main light will be diffuseColor[0].
    float4 diffuseColor[NUM_LIGHTS];
    float4 specularColor;
    float specularPower;
    float3 mainLightDirection;
    float4 ambientColor;
}
cbuffer TranslationBuffer
{
    float2 textureTranslation;
}
cbuffer TransparentBuffer
{
    float blendAmount;
}
cbuffer WaterBuffer
{
    float waterTranslation;
    float reflectRefractScale;
    float2 waterPadding;
}
cbuffer PixelBuffer 
{
    float4 pixelColor;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal: NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
    float fogFactor : FOG;
    float clip : SV_ClipDistance0;
    float4 reflectionPosition : TEXCOORD2;
    float4 refractionPosition : TEXCOORD3;
    float3 lightPos[NUM_LIGHTS] : TEXCOORD4;
};

float4 NormalMapPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor = ShaderTexture1.Sample(Sampler, input.tex);
    float4 normalMap = ShaderTexture2.Sample(Sampler, input.tex);

    //make the normal map -1 to 1 not 0 to 1 as the normal map can give negative vectors.
    normalMap = (normalMap * 2.0f) - 1.0f;
    float3 bumpNormal = normalize((normalMap.x * input.tangent) + (normalMap.y * input.binormal) + (normalMap.z * input.normal));

    //how much light will be on this pixel (-light direction to be towards light instead of towards normal) 
    float lightIntensity = saturate(dot(bumpNormal, -mainLightDirection));
    float4 color = saturate(diffuseColor[0] * lightIntensity) * textureColor;
    return color;
}


float4 SpecularMapPixelShader(PixelInputType input) : SV_TARGET
{
    //This first section is the same as the normal map above.
    float4 textureColor = ShaderTexture1.Sample(Sampler, input.tex);
    float4 normalMap = ShaderTexture2.Sample(Sampler, input.tex);
    normalMap = (normalMap * 2.0f) - 1.0f;
    float3 bumpNormal = normalize((normalMap.x * input.tangent) + (normalMap.y * input.binormal) + (normalMap.z * input.normal));
    float lightIntensity = saturate(dot(bumpNormal, -mainLightDirection));
    float4 color = saturate(diffuseColor[0] * lightIntensity) * textureColor;

    if (lightIntensity <= 0.0f)
        return color;

    float4 specularIntensity = ShaderTexture3.Sample(Sampler, input.tex);

    //Make sure to use the normal maps so specular isnt just going off a flat surface.
    //reflection is the direction.
    float3 reflection = normalize(2 * lightIntensity * bumpNormal + mainLightDirection);
    //limit the dot of the reflection direction and the view direction and then multiply by the preset specular power amount)
    float4 specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower) * specularIntensity;
    color = saturate(color + specular);

    return color;
}
 

//float4 TextureSingleSamplePixelShader(PixelInputType a_input) : SV_TARGET
//{
//    //sample the color from the texture using the sampler at this texture coordinate location.
//    //float4 textureColor = ShaderTexture1.Sample(Sampler, a_input.tex);
//    //return textureColor;
//}
//
//float4 TextureMultiLightPixelShader(PixelInputType a_input) : SV_TARGET
//{
//    //float lightIntensity[NUM_LIGHTS];
//    //float4 colorArray[NUM_LIGHTS];
//    //float4 colorSum = float4(0.0f,0.0f,0.0f,0.0f);
//    //float4 textureColor = ShaderTexture1.Sample(Sampler, a_input.tex);
//    //
//    ////for (int i = 0; i < NUM_LIGHTS; i++)
//    ////{
//    ////    lightIntensity[i] = saturate(dot(a_input.normal, a_input.directionToLight[i]));
//    ////    //colorArray[i] = LightColor[i] * lightIntensity[i];
//    ////}
//    //
//    //for (int j = 0; j < NUM_LIGHTS; j++)
//    //{
//    //    colorSum.r += colorArray[j].r;
//    //    colorSum.g += colorArray[j].g;
//    //    colorSum.b += colorArray[j].b;
//    //}
//    //float4 color = saturate(colorSum) * textureColor;
//    //return color;
//}
//
//float4 TextureMultiSamplePixelShader(PixelInputType a_input) : SV_TARGET
//{
//    //float4 color1 = ShaderTexture1.Sample(Sampler, a_input.tex);
//    //float4 color2 = ShaderTexture2.Sample(Sampler, a_input.tex);
//    //float4 blendColor = saturate(color1 * color2 * 2.0);
//    //return blendColor;
//}
//
//float4 TextureLightMapPixelShader(PixelInputType a_input) : SV_TARGET
//{
//    //float4 color1 = ShaderTexture1.Sample(Sampler, a_input.tex);
//    //float4 lightColor = ShaderTexture2.Sample(Sampler, a_input.tex);
//    //float4 blendColor = (color1 * lightColor);
//    //return blendColor;
//}
//
//float4 TextureAlphaMapPixelShader(PixelInputType a_input) : SV_TARGET
//{
//    //float4 color1 = ShaderTexture1.Sample(Sampler, a_input.tex);
//    //float4 color2 = ShaderTexture2.Sample(Sampler, a_input.tex);
//    //float4 alphaValue = ShaderTexture3.Sample(Sampler, a_input.tex);
//    ////By doing 1-alpha its a flip on the values eg alpha 0.1 will remove 90% of color while giving 1-0.1(0.9) to color2 eg 90% of its color.
//    //float4 blendColor = saturate((alphaValue * color1) + ((1.0 - alphaValue) * color2));
//    //return blendColor;
//}
//
//float4 TextureFontSamplePixelShader(PixelInputType a_input) : SV_TARGET
//{
//    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
//    //float4 textureColor = ShaderTexture1.Sample(Sampler, a_input.tex);
//    //
//    //if (textureColor.r == 0.0f)
//    //{
//    //    textureColor.a = 0.0f;
//    //}
//    //else
//    //{
//    //    textureColor.a = 1.0f;
//    //    //textureColor = textureColor * pixelColor;
//    //}
//    //
//    //return textureColor;
//}
//
//float4 TextureRawColorPixelShader(PixelInputType a_input) : SV_TARGET
//{
//    //return a_input.color;
//}


