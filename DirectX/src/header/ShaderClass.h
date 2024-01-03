#pragma once

const int NUM_LIGHTS = 4;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class ShaderClass
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };
    struct LightInformationBufferType 
    {
        XMFLOAT4 lightPosition[NUM_LIGHTS];
        XMFLOAT4 lightDiffuse[NUM_LIGHTS];
    };
    struct PixelBufferType
    {
        XMFLOAT4 pixelColor;
    };
    
public:

    ShaderClass();
    ShaderClass(const ShaderClass& a_Copy);
    ~ShaderClass();

    bool Initialize(ID3D11Device* a_Device, HWND a_WindowHandle, int a_blendAmount, bool a_allowLights);
    void Shutdown();
    
    bool Render(ID3D11DeviceContext* a_deviceContext,
                int a_indexCount,
                ID3D11ShaderResourceView* a_texture1,
                ID3D11ShaderResourceView* a_texture2,
                ID3D11ShaderResourceView* a_texture3,
                XMMATRIX a_world,
                XMMATRIX a_view,
                XMMATRIX a_projection,
                XMFLOAT4 a_lightPosition[NUM_LIGHTS],
                XMFLOAT4 a_lightDiffuse[NUM_LIGHTS]);
    
    ID3D11ShaderResourceView* GetTexture(int a_textureNumber);

    bool AllowsLights();
    bool HasBlendingEnabled();
    
private:
    
    bool InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* a_vsFileName, WCHAR* a_psFileName, int a_amountOfBlendTextures);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, WCHAR* a_FilePath);

    bool SetShaderParams(ID3D11DeviceContext* a_DeviceContext,
                         ID3D11ShaderResourceView* a_Texture1,
                         ID3D11ShaderResourceView* a_Texture2,
                         ID3D11ShaderResourceView* a_Texture3,
                         XMMATRIX a_world,
                         XMMATRIX a_view,
                         XMMATRIX a_projection,
                         XMFLOAT4 a_lightPosition[NUM_LIGHTS],
                         XMFLOAT4 a_lightDiffuse[NUM_LIGHTS]);
    
    void RenderShader(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount);

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_InputLayout;
    
    //Used to interface with the texture shader.
    ID3D11SamplerState* m_SampleState;

    ID3D11Buffer* m_MatrixBuffer;
    ID3D11Buffer* m_LightInformationBuffer;
    ID3D11Buffer* m_PixelBuffer;

    //Setup for multi texture blending. Limited to 2 blending textures at the moment.
    ID3D11ShaderResourceView* m_Texture;
    ID3D11ShaderResourceView* m_BlendTexture1;
    ID3D11ShaderResourceView* m_BlendTexture2;

    bool m_AllowLights;
};
