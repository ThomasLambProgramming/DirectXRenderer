//Globals
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex: TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType TextureVertexShader(VertexInputType a_Input)
{
    PixelInputType output;

    output.position(a_Input.position, worldMatrix);
    output.position(a_Input.position, viewMatrix);
    output.position(a_Input.position, projectionMatrix);

    output.tex = a_Input.tex;

    return output;
}
