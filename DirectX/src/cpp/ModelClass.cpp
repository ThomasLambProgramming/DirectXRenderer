#include "ModelClass.h"

ModelClass::ModelClass()
{
    m_VertexBuffer = nullptr;
    m_IndexBuffer = nullptr;
	m_texture = nullptr;
	m_Model = nullptr;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_TextureFileName, char* a_ModelFileName, char* a_TextureFileName2, char* a_TextureFileName3, char* a_TextureFileName4, char* a_TextureFileName5)
{
    bool result;

	//load the model data
	result = LoadModel(a_ModelFileName);
	if (!result)
	{
		return false;
	}

	//calculate the tangent and binormals of the model
	CalculateModelVectors();
	
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
	if (a_TextureFileName2)
	{
		result = LoadTexture(a_Device, a_DeviceContext, a_TextureFileName2, 1);
		if (!result)
		{
			return false;
		}
	}
	if (a_TextureFileName3)
	{
		result = LoadTexture(a_Device, a_DeviceContext, a_TextureFileName3, 2);
		if (!result)
		{
			return false;
		}
	}
	if (a_TextureFileName4)
	{
		result = LoadTexture(a_Device, a_DeviceContext, a_TextureFileName4, 3);
		if (!result)
		{
			return false;
		}
	}
	if (a_TextureFileName5)
	{
		result = LoadTexture(a_Device, a_DeviceContext, a_TextureFileName5, 4);
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
		return m_texture->GetTexture();
	if (a_texture == 1 && m_SecondaryTexture1)
		return m_SecondaryTexture1->GetTexture();
	if (a_texture == 2 && m_SecondaryTexture2)
		return m_SecondaryTexture2->GetTexture();
	if (a_texture == 3 && m_SecondaryTexture3)
		return m_SecondaryTexture3->GetTexture();
	if (a_texture == 4 && m_SecondaryTexture4)
		return m_SecondaryTexture4->GetTexture();
	
	return m_texture->GetTexture();
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
		m_Model = nullptr;
	}
	return;
}

bool ModelClass::LoadTexture(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_FileName, int a_texId)
{
	bool result;

	if (a_texId == 0)
	{
		//create and initalize the texture object;
		m_texture = new TextureClass;
		result = m_texture->Initialize(a_Device, a_DeviceContext, a_FileName);
	}
	if (a_texId == 1)
	{
		//create and initalize the texture object;
		m_SecondaryTexture1 = new TextureClass;
		result = m_SecondaryTexture1->Initialize(a_Device, a_DeviceContext, a_FileName);
	}
	if (a_texId == 2)
	{
		//create and initalize the texture object;
		m_SecondaryTexture2 = new TextureClass;
		result = m_SecondaryTexture2->Initialize(a_Device, a_DeviceContext, a_FileName);
	}
	if (a_texId == 3)
	{
		//create and initalize the texture object;
		m_SecondaryTexture3 = new TextureClass;
		result = m_SecondaryTexture3->Initialize(a_Device, a_DeviceContext, a_FileName);
	}
	if (a_texId == 4)
	{
		//create and initalize the texture object;
		m_SecondaryTexture4 = new TextureClass;
		result = m_SecondaryTexture4->Initialize(a_Device, a_DeviceContext, a_FileName);
	}
	if (!result)
	{
		return false;
	}
	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = nullptr;
	}
	if (m_SecondaryTexture1)
	{
		m_SecondaryTexture1->Shutdown();
		delete m_SecondaryTexture1;
		m_SecondaryTexture1 = nullptr;
	}
	if (m_SecondaryTexture2)
	{
		m_SecondaryTexture2->Shutdown();
		delete m_SecondaryTexture2;
		m_SecondaryTexture2 = nullptr;
	}
	if (m_SecondaryTexture3)
	{
		m_SecondaryTexture3->Shutdown();
		delete m_SecondaryTexture3;
		m_SecondaryTexture3 = nullptr;
	}
	if (m_SecondaryTexture4)
	{
		m_SecondaryTexture4->Shutdown();
		delete m_SecondaryTexture4;
		m_SecondaryTexture4 = nullptr;
	}
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
		vertices[i].texture = XMFLOAT2(m_Model[i].tu, m_Model[i].tv);
		vertices[i].normal = XMFLOAT3(m_Model[i].nx, m_Model[i].ny, m_Model[i].nz);
		vertices[i].tangent = XMFLOAT3(m_Model[i].tx, m_Model[i].ty, m_Model[i].tz);
        vertices[i].binormal = XMFLOAT3(m_Model[i].bx, m_Model[i].by, m_Model[i].bz);

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
	vertices = nullptr;
	delete indices;
	indices = nullptr;
	
	return true;
}

void ModelClass::ShutdownBuffers()
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

void ModelClass::CalculateModelVectors()
{
    int faceCount, i, index;
    TempVertexType vertex1, vertex2, vertex3;
    XMFLOAT3 tangent, binormal;


    // Calculate the number of faces in the model.
    faceCount = m_VertexCount / 3;

    // Initialize the index to the model data.
    index = 0;

    // Go through all the faces and calculate the the tangent and binormal vectors.
    for(i=0; i<faceCount; i++)
    {
        // Get the three vertices for this face from the model.
        vertex1.x = m_Model[index].x;
        vertex1.y = m_Model[index].y;
        vertex1.z = m_Model[index].z;
        vertex1.tu = m_Model[index].tu;
        vertex1.tv = m_Model[index].tv;
        index++;
        
        vertex2.x = m_Model[index].x;
        vertex2.y = m_Model[index].y;
        vertex2.z = m_Model[index].z;
        vertex2.tu = m_Model[index].tu;
        vertex2.tv = m_Model[index].tv;
        index++;

        vertex3.x = m_Model[index].x;
        vertex3.y = m_Model[index].y;
        vertex3.z = m_Model[index].z;
        vertex3.tu = m_Model[index].tu;
        vertex3.tv = m_Model[index].tv;
        index++;

        // Calculate the tangent and binormal of that face.
        CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

        // Store the tangent and binormal for this face back in the model structure.
        m_Model[index-1].tx = tangent.x;
        m_Model[index-1].ty = tangent.y;
        m_Model[index-1].tz = tangent.z;
        m_Model[index-1].bx = binormal.x;
        m_Model[index-1].by = binormal.y;
        m_Model[index-1].bz = binormal.z;

        m_Model[index-2].tx = tangent.x;
        m_Model[index-2].ty = tangent.y;
        m_Model[index-2].tz = tangent.z;
        m_Model[index-2].bx = binormal.x;
        m_Model[index-2].by = binormal.y;
        m_Model[index-2].bz = binormal.z;

        m_Model[index-3].tx = tangent.x;
        m_Model[index-3].ty = tangent.y;
        m_Model[index-3].tz = tangent.z;
        m_Model[index-3].bx = binormal.x;
        m_Model[index-3].by = binormal.y;
        m_Model[index-3].bz = binormal.z;
    }

    return;
}

void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, XMFLOAT3& tangent, XMFLOAT3& binormal)
{
    float vector1[3], vector2[3];
    float tuVector[2], tvVector[2];
    float den;
    float length;


    // Calculate the two vectors for this face.
    vector1[0] = vertex2.x - vertex1.x;
    vector1[1] = vertex2.y - vertex1.y;
    vector1[2] = vertex2.z - vertex1.z;

    vector2[0] = vertex3.x - vertex1.x;
    vector2[1] = vertex3.y - vertex1.y;
    vector2[2] = vertex3.z - vertex1.z;

    // Calculate the tu and tv texture space vectors.
    tuVector[0] = vertex2.tu - vertex1.tu;
    tvVector[0] = vertex2.tv - vertex1.tv;

    tuVector[1] = vertex3.tu - vertex1.tu;
    tvVector[1] = vertex3.tv - vertex1.tv;

    // Calculate the denominator of the tangent/binormal equation.
    den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

    // Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
    tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
    tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
    tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

    binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
    binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
    binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

    // Calculate the length of this normal.
    length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

    // Normalize the normal and then store it
    tangent.x = tangent.x / length;
    tangent.y = tangent.y / length;
    tangent.z = tangent.z / length;

    // Calculate the length of this normal.
    length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

    // Normalize the normal and then store it
    binormal.x = binormal.x / length;
    binormal.y = binormal.y / length;
    binormal.z = binormal.z / length;

    return;
}
