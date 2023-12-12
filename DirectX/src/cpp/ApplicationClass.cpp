#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
    //Safety setting to 0;
    m_Direct3D = 0;
    m_Camera = 0;
    m_Model = 0;
    m_ColorShader = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass&)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initalize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;
    m_Direct3D = new Direct3DClass;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    //create camera
    m_Camera = new CameraClass;
    //set initial position of camera
    m_Camera->SetPosition(0.0f,0.0f,-5.0f);

    //create a new model
    m_Model = new ModelClass;

    result = m_Model->Initialize(m_Direct3D->GetDevice());
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK);
        return false;
    }
    //create and init the color shader;
	m_ColorShader = new ColorShaderClass;
    result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Color shader object", L"Error", MB_OK);
        return false;
    }

    
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

    // Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
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
    bool result;
    //Render Scene
    result = Render();
    
    if (!result)
    {
        return false;
    }
    
    return true;
}

bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;
    //clear buffers to begin the scene
    m_Direct3D->BeginScene(0.0f,0.0f,0.0f,1.0f);

    //update cameras view matrix
    m_Camera->Render();

    //get all the matrices
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//put the model vertex and index buffers into the graphics pipeline to prepare them to be drawn
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//render the model using the color shader
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
	
    m_Direct3D->EndScene();
    return true;
}
