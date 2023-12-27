#pragma once

const int NUM_LIGHTS = 4;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

struct MatrixBufferType
{
    XMMATRIX world;
    XMMATRIX view;
    XMMATRIX projection;
};
struct LightColorBufferType 
{
    XMFLOAT4 lightDiffuse[NUM_LIGHTS];
};
struct LightPositionBufferType
{
    XMFLOAT4 lightPosition[NUM_LIGHTS];
};

class TextureShaderClass
{
public:

    TextureShaderClass();
    TextureShaderClass(const TextureShaderClass& a_Copy);
    ~TextureShaderClass();

    bool Initialize(ID3D11Device* a_Device, HWND a_WindowHandle);
    void Shutdown();
    
    bool Render(ID3D11DeviceContext* a_DeviceContext,
                int a_IndexCount,
                ID3D11ShaderResourceView* a_ShaderResourceView,
                MatrixBufferType a_MatrixBufferData,
                LightPositionBufferType a_LightPositionBufferData,
                LightColorBufferType a_LightColorBufferData);
    
    ID3D11ShaderResourceView* GetTexture() {return m_Texture;}
    
private:
    
    bool InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* a_vsFileName, WCHAR* a_psFileName);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, WCHAR* a_FilePath);

    bool SetShaderParams(ID3D11DeviceContext* a_DeviceContext,
                        ID3D11ShaderResourceView* a_Texture,
                        MatrixBufferType a_MatrixBufferData,
                        LightPositionBufferType a_LightPositionBufferData,
                        LightColorBufferType a_LightColorBufferData);
    
    void RenderShader(ID3D11DeviceContext* a_DeviceContext, int );

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_InputLayout;
    
    //Used to interface with the texture shader.
    ID3D11SamplerState* m_SampleState;

    ID3D11Buffer* m_MatrixBuffer;
    ID3D11Buffer* m_LightPositionBuffer;
    ID3D11Buffer* m_LightColorBuffer;

    ID3D11ShaderResourceView* m_Texture;
};
