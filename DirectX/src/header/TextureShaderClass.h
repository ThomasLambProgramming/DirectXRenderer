#pragma once

const int NUM_LIGHTS = 4;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class TextureShaderClass
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
        XMFLOAT4 lightDiffuse[NUM_LIGHTS];
        XMFLOAT4 lightPosition[NUM_LIGHTS];
    };
    
public:

    TextureShaderClass();
    TextureShaderClass(const TextureShaderClass& a_Copy);
    ~TextureShaderClass();

    bool Initialize(ID3D11Device* a_Device, HWND a_WindowHandle, int a_blendAmount, bool a_allowLights);
    void Shutdown();
    
    bool Render(ID3D11DeviceContext* a_DeviceContext,
                int a_IndexCount,
                ID3D11ShaderResourceView* a_ShaderResourceView,
                MatrixBufferType a_MatrixBufferData);
    
    ID3D11ShaderResourceView* GetTexture(int a_textureNumber);

    bool AllowsLights();
    bool HasBlendingEnabled();
    
private:
    
    bool InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* a_vsFileName, WCHAR* a_psFileName, int a_amountOfBlendTextures);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, WCHAR* a_FilePath);

    bool SetShaderParams(ID3D11DeviceContext* a_DeviceContext,
                        ID3D11ShaderResourceView* a_Texture,
                        MatrixBufferType a_MatrixBufferData);
    
    void RenderShader(ID3D11DeviceContext* a_DeviceContext, int );

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_InputLayout;
    
    //Used to interface with the texture shader.
    ID3D11SamplerState* m_SampleState;

    ID3D11Buffer* m_MatrixBuffer;
    ID3D11Buffer* m_LightInformationBuffer;

    ID3D11ShaderResourceView* m_Texture;
    //Setup for multi texture blending. Limited to 2 blending textures at the moment.
    ID3D11ShaderResourceView* m_BlendTexture1;
    ID3D11ShaderResourceView* m_BlendTexture2;

    bool m_AllowLights;
};
