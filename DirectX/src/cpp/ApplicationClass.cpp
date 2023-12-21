#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
    //Safety setting to 0/nullptr;
    m_Direct3D = 0;
    m_Camera = 0;
    m_Model = 0;
	m_TextureShader = 0;
	m_Lights = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass&)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initalize(int screenWidth, int screenHeight, HWND a_WindowHandle)
{
	char textureFilename[128];
	char modelFileName[128];
    bool result;
    m_Direct3D = new Direct3DClass;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, a_WindowHandle, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(a_WindowHandle, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    //create camera
    m_Camera = new CameraClass;
    //set initial position of camera
    m_Camera->SetPosition(0.0f,2.0f,-12.0f);

    //create a new model
    m_Model = new ModelClass;

	strcpy_s(textureFilename, "./data/stone01.tga");
	strcpy_s(modelFileName, "./data/plane.txt");
	
    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename, modelFileName);
    if (!result)
    {
        MessageBox(a_WindowHandle, L"Could not initialize model object", L"Error", MB_OK);
        return false;
    }
	
    //create and init the color shader;
	//m_ColorShader = new ColorShaderClass;
    //result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);

	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), a_WindowHandle);

    if (!result)
    {
        MessageBox(a_WindowHandle, L"Could not initialize texture shader object", L"Error", MB_OK);
        return false;
    }

	m_numLights = 4;
	
	m_Lights = new LightClass[m_numLights];

	for (int i = 0; i < m_numLights; i++)
	{
		m_Lights[i].m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f); //white light.
		m_Lights[i].m_AmbientColor = XMFLOAT4(0.15f,0.15f,0.15f,1.0f); //white light.
		m_Lights[i].m_LightDirection = XMFLOAT3(0.0f,0.0f,0.0f);
		m_Lights[i].m_SpecularColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
		m_Lights[i].m_SpecularPower = 32.0f;
	}
    
	m_Lights[0].m_DiffuseColor = XMFLOAT4(1.0f,0.0f,0.0f,1.0f); //white light.
	m_Lights[1].m_DiffuseColor = XMFLOAT4(0.0f,1.0f,0.0f,1.0f); //white light.
	m_Lights[2].m_DiffuseColor = XMFLOAT4(0.0f,0.0f,1.0f,1.0f); //white light.
	m_Lights[3].m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f); //white light.
	
    return true;
}

void ApplicationClass::Shutdown()
{
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }

    if (m_Lights)
    {
	    delete [] m_Lights;
    	m_Lights = 0;
    }
	
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	
	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
    return;
}

bool ApplicationClass::Frame()
{
	static float rotation = 0.0f;
    bool result;

	//(no damn clue what this number is supposed to be).
	rotation -= 0.0174532825f * 0.1f;
	
	//Render Scene
    result = Render(rotation);
    
    if (!result)
    {
        return false;
    }
    
    return true;
}

bool ApplicationClass::Render(float a_Rotation)
{
    XMMATRIX rotateMatrix, translateMatrix;

	MatrixBufferType matrixBuffer;
	LightBufferType lightBuffer[4];
	CameraBufferType cameraBuffer = CameraBufferType();
    bool result;
	
    //clear buffers to begin the scene
    m_Direct3D->BeginScene(0.0f,0.0f,0.0f,1.0f);

    //update cameras view matrix
    m_Camera->Render();

    //get all the matrices
	m_Direct3D->GetWorldMatrix(matrixBuffer.world);
	m_Camera->GetViewMatrix(matrixBuffer.view);
	m_Direct3D->GetProjectionMatrix(matrixBuffer.projection);

	rotateMatrix = XMMatrixRotationY(a_Rotation);
	translateMatrix = XMMatrixTranslation(-2.0f, 0.0f, 0.0f);
	matrixBuffer.world = XMMatrixMultiply(rotateMatrix, translateMatrix);

	//put the model vertex and index buffers into the graphics pipeline to prepare them to be drawn
	m_Model->Render(m_Direct3D->GetDeviceContext());

	cameraBuffer.cameraPosition = m_Camera->GetPosition();
	cameraBuffer.padding = 0.0f;

	for (int i = 0; i < m_numLights; i++)
	{
		lightBuffer[i].ambientColor = m_Lights[i].m_AmbientColor;
		lightBuffer[i].diffuseColor = m_Lights[i].m_DiffuseColor;
		lightBuffer[i].lightDirection = m_Lights[i].m_LightDirection; 
		lightBuffer[i].lightPosition = m_Lights[i].m_Position;
		lightBuffer[i].specularColor = m_Lights[i].m_SpecularColor;
		lightBuffer[i].specularPower = m_Lights[i].m_SpecularPower;
	}
	
	
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
									 m_Model->GetIndexCount(),
									 m_Model->GetTexture(),
									 matrixBuffer,
									 cameraBuffer,
									 lightBuffer);
	if (!result)
	{
		return false;
	}
	
    m_Direct3D->EndScene();
    return true;
}
//Exmaple of how we can add rotation and scaling to a model and render it again to make it appear like there is multiple of an object.
//scaleMatrix = XMMatrixScaling(0.5f,0.5f,0.5f);
//rotateMatrix = XMMatrixRotationX(a_Rotation);
//translateMatrix = XMMatrixTranslation(2.0f, 0.0f, 0.0f);
//
//srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
//worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);
//
//
//result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
//	m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Lights->m_LightDirection, m_Lights->m_DiffuseColor, m_Lights->m_AmbientColor, m_Camera->GetPosition(), m_Lights->m_SpecularColor, m_Lights->m_SpecularPower);
//if (!result)
//{
//	return false;
//}