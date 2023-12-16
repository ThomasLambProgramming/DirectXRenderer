#include "ModelClass.h"

ModelClass::ModelClass()
{
    m_VertexBuffer = 0;
    m_IndexBuffer = 0;
	m_Texture = 0;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_TextureFileName)
{
    bool result;
    //init the vert/index buffers
    result = InitializeBuffers(a_Device);
    if (!result)
    {
        return false;
    }

	result = LoadTexture(a_Device, a_DeviceContext, a_TextureFileName);
	if (!result)
	{
		return false;
	}
	
    return true;
}

void ModelClass::Shutdown()
{
	ReleaseTexture();
    ShutdownBuffers();
    return;
}

void ModelClass::Render(ID3D11DeviceContext* a_DeviceContext)
{
    RenderBuffers(a_DeviceContext);
}

int ModelClass::GetIndexCount()
{
    return m_IndexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::LoadTexture(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_FileName)
{
	bool result;

	//create and initalize the texture object;
	m_Texture = new TextureClass;
	result = m_Texture->Initialize(a_Device, a_DeviceContext, a_FileName);
	if (!result)
	{
		return false;
	}
	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
	return;
}

bool ModelClass::InitializeBuffers(ID3D11Device* a_Device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
    D3D11_SUBRESOURCE_DATA indexData;
    HRESULT result;

    //Set num of vertices and indices in the arrays
    m_VertexCount = 3;
    m_IndexCount = 3;

    //create vertex array
    vertices = new VertexType[m_VertexCount];
    if (!vertices)
    {
        return false;
    }
    indices = new unsigned long[m_IndexCount];
    if (!indices)
    {
        return false;
    }

    
    //bottom left
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

    //top middle
	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

    //bottom right
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].texture  = XMFLOAT2(1.0f, 1.0f);

    // Load the index array with data, small note we always go clockwise in terms of vert order otherwise it will render it the wrong way around.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

    //setup description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    //give subresource structure a pointer to the vert data
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    //now that we have the subresource(init data), buffer description and the buffer avaliable we create it.
    result = a_Device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    //now we do the same but with the index buffer;

    // Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = a_Device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	delete vertices;
	vertices = 0;
	delete indices;
	indices = 0;
	
	return true;
}

void ModelClass::ShutdownBuffers()
{
	if (m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = 0;
	}
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = 0;
	}
	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* a_DeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	//set vert buffer to active in the input assembler so it can be rendered.
	a_DeviceContext->IASetVertexBuffers(0,1,&m_VertexBuffer, &stride, &offset);
	//set the index buffer to active in the input assembler so it can be rendered.
	a_DeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//set the type of primitive that should be rendered from this vert buffer, in this case it be a triangle
	a_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return;
}
