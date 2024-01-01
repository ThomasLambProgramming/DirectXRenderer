#include "TextClass.h"

TextClass::TextClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
}

TextClass::TextClass(const TextClass& a_Copy)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* a_Device,
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
                           float a_Blue)
{
    m_screenHeight = a_ScreenHeight;
    m_screenWidth = a_ScreenWidth;
    m_maxLength = a_MaxLength;
    
    return InitializeBuffers(a_Device, a_DeviceContext, a_Font, a_Text, a_PositionX, a_PositionY, a_Red, a_Green, a_Blue);
}

void TextClass::Shutdown()
{
    ShutdownBuffers();
}

void TextClass::Render(ID3D11DeviceContext* a_DeviceContext)
{
    //but vert and index buffers into the pipeline to prepare for drawing.
    RenderBuffers(a_DeviceContext);
}

int TextClass::GetIndexCount()
{
    return m_indexCount;
}

bool TextClass::UpdateText(ID3D11DeviceContext* a_deviceContext,
                           FontClass* a_font,
                           char* a_text,
                           int a_positionX,
                           int a_positionY,
                           float a_red,
                           float a_green,
                           float a_blue)
{
    m_pixelColor = XMFLOAT4(a_red, a_green, a_blue, 1.0f);
    int numLetters = (int)strlen(a_text);
    
    if (numLetters > m_maxLength)
        return false;

    VertexType* vertices = new VertexType[m_vertexCount];
    memset(vertices, 0, sizeof(VertexType) * m_vertexCount);
    
    float drawX = (float)(m_screenWidth / 2 * -1 + a_positionX);
    float drawY = (float)(m_screenHeight / 2 - a_positionY);

    a_font->BuildVertexArray((void*)vertices, a_text, drawX, drawY);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = a_deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result))
    {
        return false;
    }

    VertexType* verticesPtr = (VertexType*)mappedResource.pData;
    memcpy(verticesPtr, (void*)vertices, sizeof(VertexType) * m_vertexCount);

    a_deviceContext->Unmap(m_vertexBuffer, 0);
    delete [] vertices;
    vertices = 0;

    return true;
}

XMFLOAT4 TextClass::GetPixelColor()
{
    return m_pixelColor;
}

bool TextClass::InitializeBuffers(ID3D11Device* a_device,
                           ID3D11DeviceContext* a_deviceContext,
                           FontClass* a_font,
                           char* a_text,
                           int a_positionX,
                           int a_positionY,
                           float a_red,
                           float a_green,
                           float a_blue)
{
    VertexType* vertices;
    unsigned long* indicies;
    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
    D3D11_SUBRESOURCE_DATA indexData;
    HRESULT result;

    m_vertexCount = 6 * m_maxLength;
    m_indexCount = m_vertexCount;

    vertices = new VertexType[m_vertexCount];
    indicies = new unsigned long[m_indexCount];

    memset(vertices, 0, sizeof(VertexType) *m_vertexCount);
    for (int i = 0; i < m_indexCount; i++)
    {
        indicies[i] = i;
    }

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.StructureByteStride = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = a_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.StructureByteStride = 0;
    indexBufferDesc.MiscFlags = 0;
    //Apparently we never need to change this according to the tutorial.
    indexBufferDesc.CPUAccessFlags = 0;

    indexData.pSysMem = indicies;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = a_device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    delete vertices;
    delete indicies;
    vertices = 0;
    indicies = 0;

    result = UpdateText(a_deviceContext, a_font, a_text, a_positionX, a_positionY, a_red, a_green, a_blue);
    if (FAILED(result))
    {
        return false;
    }
    
    return true;
}

void TextClass::ShutdownBuffers()
{
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }
    
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }
}

void TextClass::RenderBuffers(ID3D11DeviceContext* a_DeviceContext)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    a_DeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    a_DeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    a_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
