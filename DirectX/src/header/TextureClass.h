#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class TextureClass
{
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };
    
public:

    TextureClass();
    TextureClass(const TextureClass& a_Copy);
    ~TextureClass();

    bool Initialize(ID3D11Device* a_Device);
    void Shutdown();
    void Render(ID3D11DeviceContext* a_DeviceContext, XMMATRIX a_World, XMMATRIX a_View, XMMATRIX a_Projection, ID3D11ShaderResourceView* a_ShaderResourceView);
    
private:
    
    void InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* a_vsFileName, WCHAR* a_psFileName);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* a_Blob, HWND a_WindowHandle, WCHAR* a_FilePath);

    bool SetShaderParams(ID3D11DeviceContext* a_DeviceContext, XMMATRIX a_World, XMMATRIX a_View, XMMATRIX a_Projection, ID3D11ShaderResourceView* a_ShaderResourceView);
    void RenderShader(ID3D11DeviceContext* a_DeviceContext, int );

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_InputLayout;
    ID3D11Buffer* m_MatrixBuffer;

    //Used to interface with the texture shader.
    ID3D11SamplerState* m_SampleState;
};
