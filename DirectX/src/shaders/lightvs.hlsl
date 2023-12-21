//Defines.
#define NUM_LIGHTS 4

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}
cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
}
cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor[NUM_LIGHTS];
    float4 lightPosition[NUM_LIGHTS];
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
}

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType 
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
    float3 lightPosition[NUM_LIGHTS] : TEXCOORD2;
};


PixelInputType LightVertexShader(VertexInputType a_Input)
{
    int i;
    
    PixelInputType output;
    float4 worldPosition;
    
    a_Input.position.w = 1.0f;
    
    output.position = mul(a_Input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = a_Input.tex;

    //convert normal from local to world and normalize as it can be slightly off unit length.
    output.normal = normalize(mul(a_Input.normal, worldMatrix));

    worldPosition = mul(a_Input.position, worldMatrix);
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
    output.viewDirection = normalize(output.viewDirection);

    for (i = 0; i < NUM_LIGHTS; i++)
    {
        //its not really light position moreso directions to lights from the vertex position.
        //no clue why rastertek calls it positions when its a direction.
        output.lightPosition[i] = normalize(lightPosition[i].xyz - worldPosition.xyz);
    }
    
    return output;
    
}
