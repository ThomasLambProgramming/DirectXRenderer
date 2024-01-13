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
    float cameraPadding;
}
cbuffer LightPositionBuffer
{
    float4 lightPosition[NUM_LIGHTS];
}
cbuffer FogBuffer
{
    float fogStart;
    float fogEnd;
    float2 fogPadding;
}
cbuffer ClipPlaneBuffer
{
    float4 clipPlane;
}
cbuffer ReflectionBuffer
{
    matrix reflectionMatrix;
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
    float fogFactor : FOG;
    float clip : SV_ClipDistance0;
    float4 reflectionPosition : TEXCOORD2;
    float4 refractionPosition : TEXCOORD3;
    float3 lightPos[NUM_LIGHTS] : TEXCOORD4;
};


PixelInputType TextureVertexShader(VertexInputType a_input)
{
    PixelInputType output;

    //Matrix calculations require w to be 1. 
    a_input.position.w = 1.0f;

    //Place object in correct position for rendering using world, view and projection matrices.
    output.position = mul(a_input.position, worldMatrix);
    
    
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = a_input.tex;

    output.normal = normalize(mul(a_input.normal, (float3x3)worldMatrix));
    output.tangent = normalize(mul(a_input.tangent, (float3x3)worldMatrix));
    output.binormal = normalize(mul(a_input.binormal, (float3x3)worldMatrix));

    output.fogFactor = fogStart + fogEnd;
    output.clip = clipPlane.x;
    output.reflectionPosition = reflectionMatrix[0][0];
    output.refractionPosition = 0;

    float4 worldPosition = mul(a_input.position, worldMatrix);
    //Avoiding making another variable when we can just do the calculations before moving to view space.
    output.viewDirection = normalize(cameraPosition.xyz - worldPosition.xyz);
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        output.lightPos[i] = normalize(lightPosition[i].xyz - worldPosition.xyz);
    }
    
    return output;
}
