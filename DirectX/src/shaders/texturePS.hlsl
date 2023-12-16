//Globals
Texture2D shaderTexture : register(t0);

//Sampler is for selecting pixels from the texture and it alls us to modify how pixels are written to the polygon face when shaded. (example from rastertek:
//For example, if the polygon is really far away and only makes up 8 pixels on the screen then we use the sample state to figure out which pixels or what combination of pixels will actually be drawn from the original texture.)
SamplerState SampleType : register(s0);
//There is Sampler2D which combines both but requires newer version of directx so wont be fully compatible

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 TexturePixelShader(PixelInputType input)
{
    float4 output;

    //sample the color from the texture using the sampler at this texture coordinate location.
    output = shaderTexture.Sample(SampleType, input.tex);

    return output;
}
