#include "SpriteClass.h"



SpriteClass::SpriteClass()
{
    m_VertexBuffer = nullptr;
    m_IndexBuffer = nullptr;
    m_Textures = nullptr;
}

SpriteClass::SpriteClass(const SpriteClass&)
{
}

SpriteClass::~SpriteClass()
{
}

bool SpriteClass::Initialize(ID3D11Device* a_Device,
                             ID3D11DeviceContext* a_DeviceContext,
                             int a_ScreenWidth,
                             int a_ScreenHeight,
                             char* a_SpriteFilePath,
                             int a_RenderX,
                             int a_RenderY)
{
    bool result;
    m_screenHeight = a_ScreenHeight;
    m_screenWidth = a_ScreenWidth;

    m_RenderX = a_RenderX;
    m_RenderY = a_RenderY;
    m_frameTime = 0;

    //init the vertex and index buffer to hold the geometry for the texture plane.
    result = InitializeBuffers(a_Device);
    if (!result)
    {
        return false;
    }

    result = LoadTextures(a_Device, a_DeviceContext, a_SpriteFilePath);
    if (!result)
    {
        return false;
    }

    return true;
}

void SpriteClass::Shutdown()
{
    ReleaseTextures();
    ShutdownBuffers();
}

bool SpriteClass::Render(ID3D11DeviceContext* a_DeviceContext)
{
    bool result;

    //update buffer if bitmap position has changed from previous render
    result = UpdateBuffers(a_DeviceContext);
    if (!result)
    {
        return false;
    }

    //Put vertex and index buffers on graphics pipeline to prepare for drawing.
    RenderBuffers(a_DeviceContext);
    return true;
}

void SpriteClass::Update(float a_DeltaTime)
{
    m_frameTime += a_DeltaTime;
    if (m_frameTime <= m_cycleTime)
        return;

    m_frameTime -= m_cycleTime;
    m_currentTexture++;
    if (m_currentTexture == m_textureCount)
    {
        m_currentTexture = 0;
    }
    return;
}

int SpriteClass::GetIndexCount()
{
    return m_IndexCount;
}

ID3D11ShaderResourceView* SpriteClass::GetTexture()
{
    return m_Textures[m_currentTexture].GetTexture();
}

void SpriteClass::SetRenderLocation(int a_PosX, int a_PosY)
{
    m_RenderX = a_PosX;
    m_RenderY = a_PosY;
}

bool SpriteClass::InitializeBuffers(ID3D11Device* a_Device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
    D3D11_SUBRESOURCE_DATA indexData;
    HRESULT result;
    int i;

    //we check against previous value so if it is in the same location it doesnt cost cycles in updating the dynamic
    //vertex buffer.
    m_prevPosX = -1;
    m_prevPosY = -1;

    //We are making a square out of two triangles so 6 points are needed. (dont think we are doing vert duping.)
    m_vertexCount = 6;
    m_IndexCount = m_vertexCount;

    vertices = new VertexType[m_vertexCount];
    indices = new unsigned long[m_IndexCount];

    memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

    for (i = 0; i < m_IndexCount; i++)
        indices[i] = i;

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = a_Device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    //Buffer doesnt change basically at all since all 6 indices will be the same.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = a_Device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
    if (FAILED(result))
    {
        return false;
    }
    delete [] vertices;
    delete [] indices;
    vertices = nullptr;
    indices = nullptr;

    return true;
}

void SpriteClass::ShutdownBuffers()
{
    if (m_IndexBuffer)
    {
        m_IndexBuffer->Release();
        m_IndexBuffer = nullptr;
    }
    if (m_VertexBuffer)
    {
        m_VertexBuffer->Release();
        m_VertexBuffer = nullptr;
    }
}

bool SpriteClass::UpdateBuffers(ID3D11DeviceContext* a_DeviceContext)
{
    float left,right,top,bottom;
    VertexType* vertices;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    VertexType* dataPtr;
    HRESULT result;

    //If the image has not moved then we dont want to update the buffer.
    if ((m_prevPosX == m_RenderX) && (m_prevPosY == m_RenderY))
        return true;

    m_prevPosX = m_RenderX;
    m_prevPosY = m_RenderY;

    vertices = new VertexType[m_vertexCount];
    left = (float)(m_screenWidth / 2) * -1 + (float)m_RenderX;
    right = left + (float)m_bitmapWidth;

    top = (float)(m_screenHeight / 2) - (float)m_RenderY;
    bottom = top - (float)m_bitmapHeight;

    //copied the vertex data as its just a pain to write out for no reason.
    // Load the vertex array with data.
    // First triangle.
    vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
    vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
    vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
    vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

    // Second triangle.
    vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
    vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
    vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
    vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

    for (int i = 0; i < m_vertexCount; i++)
    {
        vertices[i].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
        vertices[i].binormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
        vertices[i].tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
    }

    result = a_DeviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    //get a pointer to the data in the const buffer.
    dataPtr = (VertexType*)mappedResource.pData;
    //dst, src, memsize
    memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

    a_DeviceContext->Unmap(m_VertexBuffer, 0);
    dataPtr = nullptr;
    delete [] vertices;
    vertices = nullptr;
    
    return true;
}

void SpriteClass::RenderBuffers(ID3D11DeviceContext* a_DeviceContext)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(VertexType);
    offset = 0;
    
    a_DeviceContext->IASetVertexBuffers(0,1, &m_VertexBuffer, &stride, &offset);
    a_DeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    //set the type of primitive that should be rendered from this vert buffer. eg triangles
    a_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    return;
}

bool SpriteClass::LoadTextures(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_SpriteFilePath)
{
char textureFilename[128];
    ifstream fin;
    int i, j;
    char input;
    bool result;


    // Open the sprite info data file.
    fin.open(a_SpriteFilePath);
    if(fin.fail())
    {
        return false;
    }

    // Read in the number of textures.
    fin >> m_textureCount;

    // Create and initialize the texture array with the texture count from the file.
    m_Textures = new TextureClass[m_textureCount];

    // Read to start of next line.
    fin.get(input);

    // Read in each texture file name.
    for(i=0; i<m_textureCount; i++)
    {
        j=0;
        fin.get(input);
        while(input != '\n')
        {
            textureFilename[j] = input;
            j++;
            fin.get(input);
        }
        textureFilename[j] = '\0';

        // Once you have the filename then load the texture in the texture array.
        result = m_Textures[i].Initialize(a_Device, a_DeviceContext, textureFilename);
        if(!result)
        {
            return false;
        }
    }

    // Read in the cycle time.
    fin >> m_cycleTime;

    // Convert the integer milliseconds to float representation.
    m_cycleTime = m_cycleTime * 0.001f;

    // Close the file.
    fin.close();

    // Get the dimensions of the first texture and use that as the dimensions of the 2D sprite images.
    m_bitmapWidth = m_Textures[0].GetWidth();
    m_bitmapHeight = m_Textures[0].GetHeight();

    // Set the starting texture in the cycle to be the first one in the list.
    m_currentTexture = 0;

    return true;
}

void SpriteClass::ReleaseTextures()
{
    if (!m_Textures)
        return;
    
    for (int i = 0; i < m_textureCount; i++)
    {
        m_Textures[i].Shutdown();
    }
    delete [] m_Textures;
    m_Textures = nullptr;
}
