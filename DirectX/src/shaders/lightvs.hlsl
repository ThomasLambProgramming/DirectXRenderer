cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
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
};

PixelInputType LightVertexShader(VertexInputType a_Input)
{
    PixelInputType output;

    a_Input.position.w = 1.0f;
    
    output.position = mul(a_Input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = a_Input.tex;

    //convert normal from local to world and normalize as it can be slightly off unit length.
    output.normal = normalize(mul(a_Input.normal, worldMatrix));
    return output;
    
}
