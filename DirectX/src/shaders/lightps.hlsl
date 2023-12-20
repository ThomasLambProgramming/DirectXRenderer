Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
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
};

float4 LightPixelShader(PixelInputType a_Input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color = ambientColor;
    float3 reflection;
    float4 specular;

    specular = float4(0.0f,0.0f,0.0f,0.0f);
    //Sample the pixel color from the texture.
    textureColor = shaderTexture.Sample(SampleType, a_Input.tex);

    //inverse (normal dot -light = if both in same direction (dot=1) then we give full lighting.
    lightDir = -lightDirection;
    //saturate= clamp(0,1).
    lightIntensity = saturate(dot(a_Input.normal, lightDir));

    if (lightIntensity > 0.0f)
    {
        color += (diffuseColor * lightIntensity);
        //saturate the ambient and diffuse color;
        color = saturate(color);


        //Shader book and tutorial giving different ways of shading but whatever decide on that later.
        
        //float3 halfwayVector = normalize(lightDirection + viewDirection);
        //return colorReflection * specularIntensity * pow(max(0,dot(normal, halfwayVector)), specularPower);

        reflection = normalize(2.0f * lightIntensity * a_Input.normal - lightDir);
        specular = pow(saturate(dot(reflection, a_Input.viewDirection)), specularPower);
        
    }
    color = color * textureColor;
    color = saturate(color + specular);
    return color;
}