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
};

PixelInputType LightVertexShader(VertexInputType a_Input)
{
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
    
    return output;
    
}
