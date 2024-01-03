#include "ModelClass.h"

ModelClass::ModelClass()
{
    m_VertexBuffer = 0;
    m_IndexBuffer = 0;
	m_Texture = 0;
	m_Model = 0;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_TextureFileName, char* a_ModelFileName, char* a_BlendTextureFileName1, char* a_BlendTextureFileName2)
{
    bool result;

	//load the model data
	result = LoadModel(a_ModelFileName);
	if (!result)
	{
		return false;
	}
    //init the vert/index buffers
    result = InitializeBuffers(a_Device);
    if (!result)
    {
        return false;
    }

	result = LoadTexture(a_Device, a_DeviceContext, a_TextureFileName, 0);
	if (!result)
	{
		return false;
	}
	if (a_BlendTextureFileName1)
	{
		result = LoadTexture(a_Device, a_DeviceContext, a_BlendTextureFileName1, 1);
		if (!result)
		{
			return false;
		}
	}
	if (a_BlendTextureFileName2)
	{
		result = LoadTexture(a_Device, a_DeviceContext, a_BlendTextureFileName2, 2);
		if (!result)
		{
			return false;
		}
	}
	
    return true;
}

void ModelClass::Shutdown()
{
	ReleaseModel();
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

ID3D11ShaderResourceView* ModelClass::GetTexture(int a_texture)
{
	if (a_texture == 0)
		return m_Texture->GetTexture();
	if (a_texture == 1)
		return m_blendTexture1->GetTexture();
	if (a_texture == 2)
		return m_blendTexture2->GetTexture();

	return m_Texture->GetTexture();
}

bool ModelClass::LoadModel(char* a_ModelFileName)
{
	ifstream fin;
    char input;
    int i;


    // Open the model file.
    fin.open(a_ModelFileName);

    // If it could not open the file then exit.
    if(fin.fail())
    {
        return false;
    }

    // Read up to the value of vertex count.
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }

    // Read in the vertex count.
    fin >> m_VertexCount;

    // Set the number of indices to be the same as the vertex count.
    m_IndexCount = m_VertexCount;

    // Create the model using the vertex count that was read in.
    m_Model = new ModelType[m_VertexCount];

    // Read up to the beginning of the data.
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }
    fin.get(input);
    fin.get(input);

    // Read in the vertex data.
    for(i = 0; i < m_VertexCount; i++)
    {
        fin >> m_Model[i].x >> m_Model[i].y >> m_Model[i].z;
        fin >> m_Model[i].tu >> m_Model[i].tv;
        fin >> m_Model[i].nx >> m_Model[i].ny >> m_Model[i].nz;
    }

    // Close the model file.
    fin.close();

    return true;
}

void ModelClass::ReleaseModel()
{
	if (m_Model)
	{
		delete [] m_Model;
		m_Model = 0;
	}
	return;
}

bool ModelClass::LoadTexture(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_FileName, int a_texId)
{
	bool result;

	if (a_texId == 0)
	{
		//create and initalize the texture object;
		m_Texture = new TextureClass;
		result = m_Texture->Initialize(a_Device, a_DeviceContext, a_FileName);
	}
	if (a_texId == 1)
	{
		//create and initalize the texture object;
		m_blendTexture1 = new TextureClass;
		result = m_blendTexture1->Initialize(a_Device, a_DeviceContext, a_FileName);
	}
	if (a_texId == 2)
	{
		//create and initalize the texture object;
		m_blendTexture2 = new TextureClass;
		result = m_blendTexture2->Initialize(a_Device, a_DeviceContext, a_FileName);
	}
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
	int i;

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

	// Load the vertex array and index array with data.
	for(i = 0; i < m_VertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_Model[i].x, m_Model[i].y, m_Model[i].z);
		vertices[i].color = XMFLOAT3(1.0f,1.0f,1.0f);
		vertices[i].texture = XMFLOAT2(m_Model[i].tu, m_Model[i].tv);
		vertices[i].blendTexture1 = XMFLOAT2(m_Model[i].tu, m_Model[i].tv);
		vertices[i].blendTexture2 = XMFLOAT2(m_Model[i].tu, m_Model[i].tv);
		vertices[i].normal = XMFLOAT3(m_Model[i].nx, m_Model[i].ny, m_Model[i].nz);
		vertices[i].tangent = XMFLOAT3(0.0f,0.0f,0.0f);
		vertices[i].binormal = XMFLOAT3(0.0f,0.0f,0.0f);
		vertices[i].padding = XMFLOAT3(0.0f,0.0f,0.0f);

		indices[i] = i;
	}

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
