#ifndef _COLORSHADERCLASS_H_
#define _COLORSHADERCLASS_H_

//Includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class ColorShaderClass
{
private:
    //This must match our Color.vs (Vertex Shader) cbuffer type to allow for data transfer.
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };
public:
    ColorShaderClass();
    ColorShaderClass(const ColorShaderClass& copy);
    ~ColorShaderClass();

    bool Initialize(ID3D11Device* a_Device, HWND a_WindowHandle);
    void Shutdown();
    bool Render(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount, XMMATRIX a_World, XMMATRIX a_View, XMMATRIX a_Projection);

private:
    bool InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* vsFileName, WCHAR* psFileName);
    void ShutdownShader();
    //Blobs can be used as a data buffer, storing vertex, adjacency, and material information during mesh optimization and loading operations. Also,
    //these objects are used to return object code and error messages in APIs that compile vertex, geometry and pixel shaders.
    void OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, WCHAR* a_Message);

    bool SetShaderParameters(ID3D11DeviceContext* a_DeviceContext, XMMATRIX a_World, XMMATRIX a_View, XMMATRIX a_Projection);
    void RenderShader(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount);

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_Layout;
    ID3D11Buffer* m_MatrixBuffer;
};


#endif