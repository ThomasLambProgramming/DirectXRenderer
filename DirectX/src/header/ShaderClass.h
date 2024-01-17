#pragma once

const int NUM_LIGHTS = 4;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

//This is a general purpose shader class for testing and learning shaders buffers and everything inbetween without needing
//to create a class and rewrite alot of code every time.
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
        XMFLOAT2 fogPadding;
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
        XMFLOAT3 mainLightDirection;
        XMFLOAT4 ambientColor;
    };
    struct TranslationBufferType
    {
        XMFLOAT2 textureTranslation;
        XMFLOAT2 textureTranslationPadding;
    };
    struct TransparentBufferType
    {
        float blendAmount;
        XMFLOAT3 blendPadding;
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
    struct PointLightBufferType
    {
        XMFLOAT4 pointLightDiffuseColor[NUM_LIGHTS];
    };
    
public:

    ShaderClass();
    ShaderClass(const ShaderClass& a_Copy);
    ~ShaderClass();

    bool Initialize(ID3D11Device* a_Device, HWND a_WindowHandle, char* a_vertexShaderEntryPoint, char* a_pixelShaderEntryPoint);
    void Shutdown();
    
    bool Render(ID3D11DeviceContext* a_deviceContext,
                int a_indexCount,
                //VertexShaderVariables.
                const XMMATRIX& a_world,
                const XMMATRIX& a_view,
                const XMMATRIX& a_projection,
                XMFLOAT3 a_cameraPosition,
                float a_fogStart,
                float a_fogEnd,
                XMFLOAT4 a_clipPlane,
                const XMMATRIX& a_reflectionMatrix,
                XMFLOAT4 a_lightPositions[NUM_LIGHTS],
                //PixelShaderVariables
                XMFLOAT4 a_mainLightDiffuse,
                XMFLOAT4 a_specularColor,
                float a_specularPower,
                XMFLOAT3 a_mainLightDirection,
                XMFLOAT4 a_ambientColor,
                XMFLOAT2 a_translationAmount,
                float a_blendAmount,
                float a_waterTranslation,
                float a_reflectRefractScale,
                XMFLOAT4 a_pixelColor,
                XMFLOAT4 a_pointLightDiffuse[NUM_LIGHTS],
                ID3D11ShaderResourceView* a_Texture1,
                ID3D11ShaderResourceView* a_Texture2,
                ID3D11ShaderResourceView* a_Texture3,
                ID3D11ShaderResourceView* a_Texture4,
                ID3D11ShaderResourceView* a_Texture5,
                int a_textureCount = 5) const;
    
    ID3D11ShaderResourceView* GetTexture(int a_textureNumber) const;

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
                         const XMMATRIX& a_reflectionMatrix,
                         XMFLOAT4 a_lightPositions[NUM_LIGHTS],
                         //PixelShaderVariables
                         XMFLOAT4 a_mainLightDiffuse,
                         XMFLOAT4 a_specularColor,
                         float a_specularPower,
                         XMFLOAT3 a_mainLightDirection,
                         XMFLOAT4 a_ambientColor,
                         XMFLOAT2 a_translationAmount,
                         float a_blendAmount,
                         float a_waterTranslation,
                         float a_reflectRefractScale,
                         XMFLOAT4 a_pixelColor,
                         XMFLOAT4 a_pointLightDiffuse[NUM_LIGHTS],
                         ID3D11ShaderResourceView* a_Texture1,
                         ID3D11ShaderResourceView* a_Texture2,
                         ID3D11ShaderResourceView* a_Texture3,
                         ID3D11ShaderResourceView* a_Texture4,
                         ID3D11ShaderResourceView* a_Texture5,
                         int a_textureCount = 5) const;
    
    void RenderShader(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount) const;
    
    void ShutdownShader();
    static void OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, const WCHAR* a_FilePath);

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
    ID3D11Buffer* m_PointLightBuffer;
    
    //UberShader allows for multitexture blending + refraction and reflection textures.
    ID3D11ShaderResourceView* m_Texture;
    ID3D11ShaderResourceView* m_SecondaryTexture1;
    ID3D11ShaderResourceView* m_SecondaryTexture2;
    ID3D11ShaderResourceView* m_SecondaryTexture3;
    ID3D11ShaderResourceView* m_SecondaryTexture4;
};
