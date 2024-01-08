#include "ApplicationClass.h"

#include <complex>

ApplicationClass::ApplicationClass()
{
    //Safety setting to 0/nullptr;
    m_Direct3D = 0;
    m_Camera = 0;
    m_Model = 0;
	m_TextureShader = 0;
	m_fps = 0;
	m_count = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass&)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initalize(int screenWidth, int screenHeight, HWND a_WindowHandle)
{
	char textureFileName[128];
	char blendTexture1FileName[128];
	char modelFileName[128];

    bool result;

	
	
	m_startTime = timeGetTime();
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
    m_Camera->SetPosition(0.0f,0.0f,-5.0f);

    //create a new model
    m_Model = new ModelClass;

	strcpy_s(textureFileName, "./data/stone01.tga");
	strcpy_s(blendTexture1FileName, "./data/normal01.tga");
	strcpy_s(modelFileName, "./data/cube.txt");
	
	m_TextureShader = new ShaderClass;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), a_WindowHandle, 3, true);
	
    if (!result)
    {
	    MessageBox(a_WindowHandle, L"Could not initialize texture shader object", L"Error", MB_OK);
    	return false;
    }
	
    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFileName, modelFileName, blendTexture1FileName );
    if (!result)
    {
        MessageBox(a_WindowHandle, L"Could not initialize model object", L"Error", MB_OK);
        return false;
    }
	m_MainLight = new LightClass;
	m_MainLight->m_LightDirection = XMFLOAT3(0.0f,0.0f,1.0f);
	m_MainLight->m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	
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
	if (m_MainLight)
	{
		delete m_MainLight;
		m_MainLight = 0;
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

bool ApplicationClass::Frame(InputClass* a_InputClass)
{
	int mouseX, mouseY;
	bool mouseDown;

	if (a_InputClass->IsEscapePressed())
		return false;

	static float rotation = 360.0f;
	
	 // Update the rotation variable each frame.
    rotation -= 0.0174532925f * 0.25f;
    if(rotation <= 0.0f)
    {
        rotation += 360.0f;
    }

	a_InputClass->GetMouseLocation(mouseX, mouseY);
	mouseDown = a_InputClass->IsMousePressed(0);

	if (!UpdateMouseStrings(mouseX, mouseY, mouseDown))
		return false;
	
	UpdateFps();
	//Render Scene
    return Render(rotation);
}

bool ApplicationClass::Render(float a_Rotation)
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
    bool result;
	
    //clear buffers to begin the scene
    m_Direct3D->BeginScene(0.0f,0.0f,0.0f,1.0f);

    //update cameras view matrix
    m_Camera->Render();

    //get all the matrices
	m_Direct3D->GetWorldMatrix(world);
	m_Camera->GetViewMatrix(view);
	m_Direct3D->GetProjectionMatrix(projection);

	world = XMMatrixRotationY(a_Rotation);
	
	//put the model vertex and index buffers into the graphics pipeline to prepare them to be drawn
	m_Model->Render(m_Direct3D->GetDeviceContext());
	
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
									 m_Model->GetIndexCount(),
									 m_Model->GetTexture(0),
									 m_Model->GetTexture(1),
									 m_Model->GetTexture(2),
									 world,
									 view,
									 projection,
									 m_MainLight->m_DiffuseColor,
									 m_MainLight->m_LightDirection);
	if (!result)
	{
		return false;
	}

	//m_Direct3D->TurnZBufferOff();
	//m_Direct3D->EnableAlphaBlending();
	//----- Do any 2d / sprite things here, alpha blending is for text.
	//m_Direct3D->DisableAlphaBlending();
	
	//m_Direct3D->TurnZBufferOn();
    m_Direct3D->EndScene();
    return true;
}

bool ApplicationClass::UpdateFps()
{
	int fps;
	//char tempString[16], finalString[16];
	//float red, green, blue;
	//bool result;
	
	m_count++;
    	
	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;
		m_startTime = timeGetTime();
	}
	fps = m_fps;

	if (m_previousFps == fps)
	{
		return true;
	}
	m_previousFps = fps;
	
	if (fps > 99999)
	{
		fps = 99999;
	}
	//sprintf_s(tempString, "%d", fps);
	//strcpy_s(finalString, "Fps: ");
	//strcat_s(finalString, tempString);
	//if (fps >= 60)
	//{
	//	red = 0.0f;
	//	green = 1.0f;
	//	blue = 0.0f;
	//}
	//if (fps < 60)
	//{
	//	red = 1.0f;
	//	green = 1.0f;
	//	blue = 0.0f;
	//}
	//if (fps < 30)
	//{
	//	red = 1.0f;
	//	green = 0.0f;
	//	blue = 0.0f;
	//}
	//result = m_fpstext->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 50, 10, red, green, blue);
	//if (!result)
	//	return false;

	return true;
}

bool ApplicationClass::UpdateMouseStrings(int posX, int posY, bool a_MouseDown)
{
	//char tempString[16], finalString[32];
	//
	//sprintf_s(tempString, "%d", posX);
	//strcpy_s(finalString, "Mouse X: ");
	//strcat_s(finalString, tempString);

	//if (!m_MouseStrings[0].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 90, 1.0f,1.0f,1.0f))
	//	return false;
	//
	//sprintf_s(tempString, "%d", posY);
	//strcpy_s(finalString, "Mouse Y: ");
	//strcat_s(finalString, tempString);
	//
	//if (!m_MouseStrings[1].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 115, 1.0f,1.0f,1.0f))
	//	return false;
	//
	//if (a_MouseDown)
	//	strcpy_s(finalString, "Mouse Button: Yes");
	//else
	//	strcpy_s(finalString, "Mouse Button: No");
	//
	//if (!m_MouseStrings[2].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 135, 1.0f,1.0f,1.0f))
	//	return false;

	return true;
}
