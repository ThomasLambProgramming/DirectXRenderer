#pragma once
#include "FontClass.h"

class TextClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };
public:
    TextClass();
    TextClass(const TextClass& a_Copy);
    ~TextClass();

    bool Initialize(ID3D11Device* a_Device,
                    ID3D11DeviceContext* a_DeviceContext,
                    int a_ScreenWidth,
                    int a_ScreenHeight,
                    int a_MaxLength,
                    FontClass* a_Font,
                    char* a_Text,
                    int a_PositionX,
                    int a_PositionY,
                    float a_Red,
                    float a_Green,
                    float a_Blue);
    
    void Shutdown();
    void Render(ID3D11DeviceContext* a_DeviceContext);

    int GetIndexCount();

    bool UpdateText(ID3D11DeviceContext* a_deviceContext,
                    FontClass* a_font,
                    char* a_text,
                    int a_positionX,
                    int a_positionY,
                    float a_red,
                    float a_green,
                    float a_blue);
    
    XMFLOAT4 GetPixelColor();

    private:
    bool InitializeBuffers(ID3D11Device* a_device,
                           ID3D11DeviceContext* a_deviceContext,
                           FontClass* a_font,
                           char* a_text,
                           int a_positionX,
                           int a_positionY,
                           float a_red,
                           float a_green,
                           float a_blue);
    
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* a_deviceContext);

    private:
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_screenWidth, m_screenHeight, m_maxLength, m_vertexCount, m_indexCount;
    XMFLOAT4 m_pixelColor;
};
