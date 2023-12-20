#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class TextureShaderClass
{
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };
    struct LightBufferType
    {
        XMFLOAT4 ambientColor;
        XMFLOAT4 diffuseColor;
        //Look at the alightment of the variables to make sure its in chunks of 4.
        XMFLOAT3 lightDirection;
        float specularPower;

        XMFLOAT4 specularColor;
    };
    struct CameraBufferType
    {
        XMFLOAT3 cameraPosition;
        float padding;
    };
    
public:

    TextureShaderClass();
    TextureShaderClass(const TextureShaderClass& a_Copy);
    ~TextureShaderClass();

    bool Initialize(ID3D11Device* a_Device, HWND a_WindowHandle);
    void Shutdown();
    bool Render(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount, XMMATRIX a_World, XMMATRIX a_View, XMMATRIX a_Projection, ID3D11ShaderResourceView* a_ShaderResourceView, XMFLOAT3 a_LightDirection, XMFLOAT4 a_DiffuseColor, XMFLOAT4 a_AmbientColor, XMFLOAT3 a_CameraPosition, XMFLOAT4 a_SpecularColor, float a_SpecularPower);
    
private:
    
    bool InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* a_vsFileName, WCHAR* a_psFileName);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, WCHAR* a_FilePath);

    bool SetShaderParams(ID3D11DeviceContext* a_DeviceContext, XMMATRIX a_World, XMMATRIX a_View, XMMATRIX a_Projection, ID3D11ShaderResourceView* a_Texture, XMFLOAT3 a_LightDirection, XMFLOAT4 a_DiffuseColor, XMFLOAT4 a_AmbientColor, XMFLOAT3 a_CameraPosition, XMFLOAT4 a_SpecularColor, float a_SpecularPower);
    void RenderShader(ID3D11DeviceContext* a_DeviceContext, int );

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_InputLayout;
    ID3D11Buffer* m_MatrixBuffer;

    //Used to interface with the texture shader.
    ID3D11SamplerState* m_SampleState;
    ID3D11Buffer* m_LightBuffer;
    ID3D11Buffer* m_CameraBuffer;
};
