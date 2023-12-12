//Globals
//cbuffer = constant buffer, used for grouping cpu->gpu info sends to increase performance.
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//if we want more than one of the same type we have to add a number to the end such as Color0, Color1 etc.
struct VertexInputType
{
    float4 position: POSITION;
    float4 color: Color;
};

struct PixelInputType
{
    //SV= SystemValue.
    float4 position: SV_POSITION;
    float4 color: COLOR;
};

PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    //Change the position vector to be 4 units for proper matrix calculations. (needs to be 1 for points 0 for directions)
    input.position.w = 1.0f;
    
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    //Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}