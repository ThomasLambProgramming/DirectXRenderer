﻿#pragma once

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
    //Vertex Shader buffers
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };
    struct CameraBufferType
    {
        XMFLOAT3 cameraPosition;
        float padding;
    };
    struct FogBufferType
    {
        float fogStart;
        float fogEnd;
    };
    struct ClipPlaneBufferType
    {
        XMFLOAT4 clipPlane;
    };
    struct ReflectionBufferType
    {
        XMMATRIX reflectionMatrix;
    };
    struct LightPositionBufferType
    {
        XMFLOAT4 lightPosition[NUM_LIGHTS];
    };

    //Pixel shader buffers.
    struct LightInformationBufferType 
    {
        XMFLOAT4 diffuseColor;
        XMFLOAT4 specularColor;
        float specularPower;
        XMFLOAT3 lightDirection;
        XMFLOAT4 ambientColor;
    };
    struct TranslationBufferType
    {
        XMFLOAT2 textureTranslation;
    };
    struct TranparentBufferType
    {
        float blendAmount;
    };
    struct WaterBufferType
    {
        float waterTranslation;
        float reflectRefractScale;
        XMFLOAT2 waterPadding;
    };
    struct PixelBufferType
    {
        XMFLOAT4 pixelColor;
    };
    
public:

    ShaderClass();
    ShaderClass(const ShaderClass& a_Copy);
    ~ShaderClass();

    bool Initialize(ID3D11Device* a_Device, HWND a_WindowHandle, int a_blendAmount, bool a_allowLights, char* a_vertexShaderEntryPoint, char* a_pixelShaderEntryPoint);
    void Shutdown();
    
    bool Render(ID3D11DeviceContext* a_deviceContext,
                int a_indexCount,
                //VertexShaderVariables.
                XMMATRIX a_world,
                XMMATRIX a_view,
                XMMATRIX a_projection,
                XMFLOAT3 a_cameraPosition,
                float a_fogStart,
                float a_fogEnd,
                XMFLOAT4 a_clipPlane,
                XMMATRIX a_reflectionMatrix,
                XMFLOAT4 a_lightPositions[NUM_LIGHTS],
                //PixelShaderVariables
                XMFLOAT4 a_lightDiffuse[NUM_LIGHTS],
                XMFLOAT4 a_specularColor,
                float a_SpecularPower,
                XMFLOAT3 a_mainLightDirection,
                XMFLOAT4 a_ambientColor,
                XMFLOAT2 a_translationAmount,
                float a_blendAmount,
                float a_waterTranslation,
                float a_reflectRefractScale,
                XMFLOAT4 a_PixelColor,
                ID3D11ShaderResourceView* a_Texture1,
                ID3D11ShaderResourceView* a_Texture2,
                ID3D11ShaderResourceView* a_Texture3,
                ID3D11ShaderResourceView* a_Texture4,
                ID3D11ShaderResourceView* a_Texture5);
    
    ID3D11ShaderResourceView* GetTexture(int a_textureNumber);

private:
    
    bool InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* a_vsFileName, WCHAR* a_psFileName, char* a_vertexShaderEntryPoint, char* a_pixelShaderEntryPoint);

    bool SetShaderParams(ID3D11DeviceContext* a_DeviceContext,
                         //VertexShaderVariables.
                         XMMATRIX a_world,
                         XMMATRIX a_view,
                         XMMATRIX a_projection,
                         XMFLOAT3 a_cameraPosition,
                         float a_fogStart,
                         float a_fogEnd,
                         XMFLOAT4 a_clipPlane,
                         XMMATRIX a_reflectionMatrix,
                         XMFLOAT4 a_lightPositions[NUM_LIGHTS],
                         //PixelShaderVariables
                         XMFLOAT4 a_lightDiffuse[NUM_LIGHTS],
                         XMFLOAT4 a_specularColor,
                         float a_SpecularPower,
                         XMFLOAT3 a_mainLightDirection,
                         XMFLOAT4 a_ambientColor,
                         XMFLOAT2 a_translationAmount,
                         float a_blendAmount,
                         float a_waterTranslation,
                         float reflectRefractScale,
                         XMFLOAT4 a_PixelColor,
                         ID3D11ShaderResourceView* a_Texture1,
                         ID3D11ShaderResourceView* a_Texture2,
                         ID3D11ShaderResourceView* a_Texture3,
                         ID3D11ShaderResourceView* a_Texture4,
                         ID3D11ShaderResourceView* a_Texture5);
    
    void RenderShader(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount);
    
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, WCHAR* a_FilePath);

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_InputLayout;
    
    //Used to interface with the texture shader, We arent doing any special effects with sampling so we only require one sampler.
    ID3D11SamplerState* m_SampleState;

    //Vertex Shader Buffers.
    ID3D11Buffer* m_MatrixBuffer;
    ID3D11Buffer* m_CameraBuffer;
    ID3D11Buffer* m_FogBuffer;
    ID3D11Buffer* m_ClipPlaneBuffer;
    ID3D11Buffer* m_ReflectionBuffer;
    ID3D11Buffer* m_LightPositionBuffer;

    //Pixel Shader Buffers.
    
    ID3D11Buffer* m_LightInformationBuffer;
    ID3D11Buffer* m_TranslationBuffer;
    ID3D11Buffer* m_TransparentBuffer;
    ID3D11Buffer* m_WaterBuffer;
    ID3D11Buffer* m_PixelBuffer;
    
    //UberShader allows for multitexture blending + refraction and reflection textures.
    ID3D11ShaderResourceView* m_Texture;
    ID3D11ShaderResourceView* m_SecondaryTexture1;
    ID3D11ShaderResourceView* m_SecondaryTexture2;
    ID3D11ShaderResourceView* m_SecondaryTexture3;
    ID3D11ShaderResourceView* m_SecondaryTexture4;

    bool m_AllowLights;
};
