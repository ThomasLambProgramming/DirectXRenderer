#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
    //Safety setting to 0/nullptr;
    m_Direct3D = 0;
    m_Camera = 0;
    m_Model = 0;
	m_TextureShader = 0;
	m_LightClass = 0;
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
    m_Camera->SetPosition(0.0f,0.0f,-8.0f);

    //create a new model
    m_Model = new ModelClass;

	strcpy_s(textureFilename, "./data/stone01.tga");
	strcpy_s(modelFileName, "./data/cube.txt");
	
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

	m_LightClass = new LightClass;
	m_LightClass->m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f); //white light.
	m_LightClass->m_LightDirection = XMFLOAT3(0.0f,0.0f,1.0f); // forwards.
    
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

    if (m_LightClass)
    {
	    delete m_LightClass;
    	m_LightClass = 0;
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
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix, scaleMatrix, srMatrix;
    bool result;
    //clear buffers to begin the scene
    m_Direct3D->BeginScene(0.0f,0.0f,0.0f,1.0f);

    //update cameras view matrix
    m_Camera->Render();

    //get all the matrices
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	rotateMatrix = XMMatrixRotationY(a_Rotation);
	translateMatrix = XMMatrixTranslation(-2.0f, 0.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);
	
	//put the model vertex and index buffers into the graphics pipeline to prepare them to be drawn
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//render the model using the color shader
	//result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
		m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_LightClass->m_LightDirection, m_LightClass->m_DiffuseColor);
	if (!result)
	{
		return false;
	}

	scaleMatrix = XMMatrixScaling(0.5f,0.5f,0.5f);
	rotateMatrix = XMMatrixRotationX(a_Rotation);
	translateMatrix = XMMatrixTranslation(2.0f, 0.0f, 0.0f);

	srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
	worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);


	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
		m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_LightClass->m_LightDirection, m_LightClass->m_DiffuseColor);
	if (!result)
	{
		return false;
	}

	
    m_Direct3D->EndScene();
    return true;
}
