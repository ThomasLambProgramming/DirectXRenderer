#include "ObjectClass.h"

ObjectClass::ObjectClass()
{
	m_VertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_texture = nullptr;
	m_model = nullptr;
	m_VertexCount = 0;
	m_IndexCount = 0;
	m_textureCount = 0;
	m_position = XMFLOAT3(0.0f,0.0f,0.0f);
	m_rotation = XMFLOAT3(0.0f,0.0f,0.0f);
	m_scale = XMFLOAT3(0.0f,0.0f,0.0f);
	m_prevPosX = 0;
	m_prevPosY = 0;
	m_2DHeight = 0;
	m_2DWidth = 0;
	m_screenHeight = 0;
	m_screenWidth = 0;
	m_objectType = ThreeDimensional;
}
//Copy constructor and destructor definitions to avoid auto generated versions.
ObjectClass::ObjectClass(const ObjectClass& a_copy): m_position(), m_rotation(), m_scale(), m_VertexBuffer(nullptr),
                                                     m_IndexBuffer(nullptr),
                                                     m_VertexCount(0), m_IndexCount(0), m_screenWidth(0),
                                                     m_screenHeight(0), m_2DWidth(0),
                                                     m_2DHeight(0),
                                                     m_prevPosX(0),
                                                     m_prevPosY(0),
                                                     m_texture(nullptr),
                                                     m_textureCount(0), m_model(nullptr), m_objectType()
{
}

ObjectClass::~ObjectClass() {}

bool ObjectClass::Initialize(ID3D11Device* a_device, ID3D11DeviceContext* a_deviceContext, const char* a_modelFileName, char* a_textureFileNames[], int a_textureCount)
{
	const string testString = string(a_modelFileName);
	bool result;
	
	if (testString.find(".txt") != string::npos)
	{
		result = LoadModelTxt(a_modelFileName);
		if (!result)
			return false;
	}
	else if (testString.find(".obj") != string::npos)
	{
		result = LoadModelObj(a_modelFileName);
		if (!result)
			return false;
	}
	else if (testString.find(".fbx") != string::npos)
	{
		result = LoadModelFbx(a_modelFileName);
		if (!result)
			return false;
	}

	//calculate the tangent and binormal of the model
	CalculateModelVectors();
	
    //init the vert/index buffers
    result = InitializeBuffers(a_device);
    if (!result)
        return false;

	m_texture = new TextureClass[MAX_TEXTURES];

	for (int i = 0; i < a_textureCount; i++)
	{
		result = LoadTexture(a_device, a_deviceContext, a_textureFileNames[i], i);
		if (!result)
			return false;
		m_textureCount++;
	}
	
    return true;
}

bool ObjectClass::InitializePrimitive(ID3D11Device* a_device, ID3D11DeviceContext* a_deviceContext, PrimitiveType a_primitive, char* a_textureFileNames[], int a_textureCount)
{
	char modelFileName[128];
	strcpy_s(modelFileName, "./data/");
	m_objectType = ThreeDimensional;
	switch (a_primitive)
	{
		case Cube:
			strcat_s(modelFileName, "cube.txt");
			break;
		case Sphere:
			strcat_s(modelFileName, "sphere.txt");
			break;
		case Plane:
			strcat_s(modelFileName, "plane.txt");
			break;
		case Quad:
			strcat_s(modelFileName, "square.txt");
			m_objectType = TwoDimensional;
			break;
		case Capsule:
			strcat_s(modelFileName, "cube.txt");
			break;
		case Cylinder:
			strcat_s(modelFileName, "cube.txt");
			break;
		default: 
			strcat_s(modelFileName, "cube.txt");
			break;
	}
	return Initialize(a_device, a_deviceContext, modelFileName, a_textureFileNames, a_textureCount);
}

void ObjectClass::SetAsObjectToRender(ID3D11DeviceContext* a_deviceContext) const
{
    SetVertexIndexBuffers(a_deviceContext);
}

int ObjectClass::GetIndexCount() const
{
    return m_IndexCount;
}

int ObjectClass::GetVertexCount() const
{
	return m_VertexCount;
}

ObjectClass::ModelInformation* ObjectClass::GetModelData() const
{
	return m_model;
}

ID3D11ShaderResourceView* ObjectClass::GetTexture(int a_texture) const
{
	if (a_texture + 1 > m_textureCount || a_texture < 0)
		return nullptr;
	
	return m_texture[a_texture].GetTexture();
}

// ReSharper disable once CppMemberFunctionMayBeConst again, its modifying member variables why are you trying to set it to const rider!
bool ObjectClass::SetNewTextureAtId(ID3D11Device* a_device, ID3D11DeviceContext* a_deviceContext, int a_texId, char* a_textureFileName)
{
	//We never want to go over max textures for one object as it will break when making buffers.
	if (a_texId> MAX_TEXTURES-1 || a_texId < 0)
		return false;
	m_texture[a_texId] = TextureClass();
	return m_texture[a_texId].Initialize(a_device, a_deviceContext, a_textureFileName);
}

//remove from font and back arent used at the moment and will only implement if needed by certain shaders or effects as needed.
void ObjectClass::RemoveTextureFromFront()
{
	if (m_textureCount == 0)
		return;

	m_texture[0].Shutdown();
	m_textureCount--;
	
	if (m_textureCount == 0)
		return;
	//If there is still textures we want to shift the array down one so the 0 element is not null.
	for (int i = 0; i < m_textureCount; i++)
	{
		m_texture[i] = m_texture[i+1];
	}
}

void ObjectClass::RemoveTextureFromBack()
{
	if (m_textureCount == 0)
		return;

	m_texture[m_textureCount-1].Shutdown();
	m_textureCount--;
	//No need for sorting as we have removed from the back.
}

XMFLOAT3 ObjectClass::GetPosition() const
{
	return m_position;
}

XMFLOAT3 ObjectClass::GetRotation() const
{
	return m_rotation;
}

XMFLOAT3 ObjectClass::GetScale() const
{
	return m_scale;
}

void ObjectClass::SetPosition(XMFLOAT3 a_value)
{
	m_position = a_value;
}

void ObjectClass::SetRotation(XMFLOAT3 a_value)
{
	m_rotation = a_value;
}

void ObjectClass::SetScale(XMFLOAT3 a_value)
{
	m_scale = a_value;
}

void ObjectClass::Shutdown()
{
	ReleaseModel();
	ReleaseTexture();
    ShutdownBuffers();

	m_position = XMFLOAT3(0.0f,0.0f,0.0f);
	m_rotation = XMFLOAT3(0.0f,0.0f,0.0f);
	m_scale = XMFLOAT3(0.0f,0.0f,0.0f);
}

bool ObjectClass::LoadModelTxt(const char* a_modelFileName)
{
	ifstream fin;
    char input;

    fin.open(a_modelFileName);
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
    m_IndexCount = m_VertexCount;
    m_model = new ModelInformation[m_VertexCount];

    // Read up to the beginning of the data.
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }
    fin.get(input);
    fin.get(input);

    //Load in all vertex data 
    for(int i = 0; i < m_VertexCount; i++)
    {
        fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
        fin >> m_model[i].tu >> m_model[i].tv;
        fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
    }

    fin.close();
    return true;
}

bool ObjectClass::LoadModelObj(const char* a_modelFileName)
{
	return false;
}

bool ObjectClass::LoadModelFbx(const char* a_modelFileName)
{
	return false;
}

bool ObjectClass::InitializeBuffers(ID3D11Device* a_device)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
    D3D11_SUBRESOURCE_DATA indexData;

    //create vertex array
    VertexType* vertices = new VertexType[m_VertexCount];
    if (!vertices)
    {
        return false;
    }
    unsigned long* indices = new unsigned long[m_IndexCount];
    if (!indices)
    {
        return false;
    }

	// Load the vertex array and index array with data.
	for(int i = 0; i < m_VertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].tangent = XMFLOAT3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
        vertices[i].binormal = XMFLOAT3(m_model[i].bx, m_model[i].by, m_model[i].bz);

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

    //now that we have the subresource(init data), buffer description and the buffer available we create it.
    HRESULT result = a_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
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
	result = a_device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
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

void ObjectClass::SetVertexIndexBuffers(ID3D11DeviceContext* a_deviceContext) const
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	//set vert buffer to active in the input assembler so it can be rendered.
	a_deviceContext->IASetVertexBuffers(0,1,&m_VertexBuffer, &stride, &offset);
	//set the index buffer to active in the input assembler so it can be rendered.
	a_deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//set the type of primitive that should be rendered from this vert buffer, in this case it be a triangle
	a_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//Rider suggests it to be const but it modifies m_texture so its not correct I believe
// ReSharper disable once CppMemberFunctionMayBeConst
bool ObjectClass::LoadTexture(ID3D11Device* a_device, ID3D11DeviceContext* a_deviceContext, char* a_textureName, int a_texId)
{
	//We never want to go over max textures for one object as it will break when making buffers.
	if (a_texId > MAX_TEXTURES-1 || a_texId < 0)
		return false;
	
	//create and initialize the texture object;
	m_texture[a_texId] = TextureClass();
	return m_texture[a_texId].Initialize(a_device, a_deviceContext, a_textureName);
}

//Rider keeps giving me this warning about the function being const is good but it modifies m_Model so that's wrong no?
// ReSharper disable once CppMemberFunctionMayBeConst
void ObjectClass::CalculateModelVectors()
{
	TempVertexType vertex1, vertex2, vertex3;
    XMFLOAT3 tangent, binormal;


    // Calculate the number of faces in the model.
	const int faceCount = m_VertexCount / 3;

    // Initialize the index to the model data.
    int index = 0;

    // Go through all the faces and calculate the the tangent and binormal vectors.
    for (int i = 0; i < faceCount; i++)
    {
        // Get the three vertices for this face from the model.
        vertex1.x = m_model[index].x;
        vertex1.y = m_model[index].y;
        vertex1.z = m_model[index].z;
        vertex1.tu = m_model[index].tu;
        vertex1.tv = m_model[index].tv;
        index++;
        
        vertex2.x = m_model[index].x;
        vertex2.y = m_model[index].y;
        vertex2.z = m_model[index].z;
        vertex2.tu = m_model[index].tu;
        vertex2.tv = m_model[index].tv;
        index++;

        vertex3.x = m_model[index].x;
        vertex3.y = m_model[index].y;
        vertex3.z = m_model[index].z;
        vertex3.tu = m_model[index].tu;
        vertex3.tv = m_model[index].tv;
        index++;

        // Calculate the tangent and binormal of that face.
        CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

        // Store the tangent and binormal for this face back in the model structure.
        m_model[index-1].tx = tangent.x;
        m_model[index-1].ty = tangent.y;
        m_model[index-1].tz = tangent.z;
        m_model[index-1].bx = binormal.x;
        m_model[index-1].by = binormal.y;
        m_model[index-1].bz = binormal.z;

        m_model[index-2].tx = tangent.x;
        m_model[index-2].ty = tangent.y;
        m_model[index-2].tz = tangent.z;
        m_model[index-2].bx = binormal.x;
        m_model[index-2].by = binormal.y;
        m_model[index-2].bz = binormal.z;

        m_model[index-3].tx = tangent.x;
        m_model[index-3].ty = tangent.y;
        m_model[index-3].tz = tangent.z;
        m_model[index-3].bx = binormal.x;
        m_model[index-3].by = binormal.y;
        m_model[index-3].bz = binormal.z;
    }
}

//Made 2 attempts at this function copying down and modifying from tutorials but errors kept coming up. 
void ObjectClass::CalculateTangentBinormal(const TempVertexType& a_vertex1, const TempVertexType& a_vertex2, const TempVertexType& a_vertex3, XMFLOAT3& a_tangent, XMFLOAT3& a_binormal)
{
    float vector1[3], vector2[3];
    float tuVector[2], tvVector[2];

    // Calculate the two vectors for this face.
    vector1[0] = a_vertex2.x - a_vertex1.x;
    vector1[1] = a_vertex2.y - a_vertex1.y;
    vector1[2] = a_vertex2.z - a_vertex1.z;

    vector2[0] = a_vertex3.x - a_vertex1.x;
    vector2[1] = a_vertex3.y - a_vertex1.y;
    vector2[2] = a_vertex3.z - a_vertex1.z;

    // Calculate the tu and tv texture space vectors.
    tuVector[0] = a_vertex2.tu - a_vertex1.tu;
    tvVector[0] = a_vertex2.tv - a_vertex1.tv;

    tuVector[1] = a_vertex3.tu - a_vertex1.tu;
    tvVector[1] = a_vertex3.tv - a_vertex1.tv;

    // Calculate the denominator of the tangent/binormal equation.
    const float den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

    // Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
    a_tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
    a_tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
    a_tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

    a_binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
    a_binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
    a_binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

    // Calculate the length of this normal.
    float length = sqrt((a_tangent.x * a_tangent.x) + (a_tangent.y * a_tangent.y) + (a_tangent.z * a_tangent.z));

    // Normalize the normal and then store it
    a_tangent.x = a_tangent.x / length;
    a_tangent.y = a_tangent.y / length;
    a_tangent.z = a_tangent.z / length;

    // Calculate the length of this normal.
    length = sqrt((a_binormal.x * a_binormal.x) + (a_binormal.y * a_binormal.y) + (a_binormal.z * a_binormal.z));

    // Normalize the normal and then store it
    a_binormal.x = a_binormal.x / length;
    a_binormal.y = a_binormal.y / length;
    a_binormal.z = a_binormal.z / length;
}

void ObjectClass::ShutdownBuffers()
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

void ObjectClass::ReleaseTexture()
{
	if (!m_texture)
	{
		return;
	}	
	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (i + 1 <= m_textureCount)
			m_texture[i].Shutdown();
	}
	delete [] m_texture;
	m_texture = nullptr;
	m_textureCount = 0;
}

void ObjectClass::ReleaseModel()
{
	if (m_model)
	{
		delete [] m_model;
		m_model = nullptr;
	}
}


