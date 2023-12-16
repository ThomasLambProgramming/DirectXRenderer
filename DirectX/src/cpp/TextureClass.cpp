#include "TextureClass.h"

TextureClass::TextureClass()
{
    m_VertexShader = 0;
    m_PixelShader = 0;
    m_InputLayout = 0;
    m_MatrixBuffer = 0;
    m_SampleState = 0;
}

TextureClass::TextureClass(const TextureClass& a_Copy)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* a_Device)
{
    //bool result = true;
    //wchar_t vsFileName[128];
    //wchar_t psFileName[128] = L"";
    //int error;
    //
    //error = wcscpy_s(vsFileName, 128, L"./");

    return true;
}

void TextureClass::Shutdown()
{
}

void TextureClass::Render(ID3D11DeviceContext* a_DeviceContext, XMMATRIX a_World, XMMATRIX a_View,
                          XMMATRIX a_Projection, ID3D11ShaderResourceView* a_ShaderResourceView)
{
}

void TextureClass::InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* a_vsFileName,
                                    WCHAR* a_psFileName)
{
}

void TextureClass::ShutdownShader()
{
}

void TextureClass::OutputShaderErrorMessage(ID3D10Blob* a_Blob, HWND a_WindowHandle, WCHAR* a_FilePath)
{
}

bool TextureClass::SetShaderParams(ID3D11DeviceContext* a_DeviceContext, XMMATRIX a_World, XMMATRIX a_View,
                                   XMMATRIX a_Projection, ID3D11ShaderResourceView* a_ShaderResourceView)
{
    return true;
}

void TextureClass::RenderShader(ID3D11DeviceContext* a_DeviceContext, int)
{
}
