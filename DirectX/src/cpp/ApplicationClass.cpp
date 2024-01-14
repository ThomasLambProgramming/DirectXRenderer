#include "ApplicationClass.h"

#include <complex>

ApplicationClass::ApplicationClass()
{
    //Safety setting to 0/nullptr;
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_Model = nullptr;
	m_TextureShader = nullptr;
	m_MainLight = nullptr;
	m_startTime = 0;
	m_previousFps = 0;
	m_fps = 0;
	m_count = 0;

}

ApplicationClass::ApplicationClass(const ApplicationClass&): m_Direct3D(nullptr), m_Camera(nullptr), m_Model(nullptr),
                                                             m_MainLight(nullptr),
                                                             m_TextureShader(nullptr), m_fps(0),
                                                             m_count(0),
                                                             m_startTime(0), m_previousFps(0)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(const int a_screenWidth, const int a_screenHeight, const HWND a_windowHandle)
{
	char textureFileName[128];
	char blendTexture1FileName[128];
	char blendTexture2FileName[128];
	char blendTexture3FileName[128];
	char blendTexture4FileName[128];
	char modelFileName[128];
	char shaderVertexEntryPoint[128];
	char shaderPixelEntryPoint[128];

	m_startTime = timeGetTime();
    m_Direct3D = new Direct3DClass;

    bool result = m_Direct3D->Initialize(a_screenWidth, a_screenHeight, VSYNC_ENABLED, a_windowHandle, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(a_windowHandle, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    //create camera
    m_Camera = new CameraClass;
    //set initial position of camera
    m_Camera->SetPosition(0.0f,1.0f,-6.0f);
    m_Camera->SetRotation(20.0f,0.0f,0.0f);

    //create a new model
    m_Model = new ModelClass;

	PixelShaderEntryPoint pixelEntry = SimpleLightingPixelShader;
	
	strcpy_s(textureFileName, "./data/stone02.tga");
	strcpy_s(blendTexture1FileName, "./data/normal02.tga");
	strcpy_s(blendTexture2FileName, "./data/spec02.tga");
	strcpy_s(blendTexture3FileName, "./data/alpha01.tga");
	strcpy_s(blendTexture4FileName, "./data/font01.tga");
	strcpy_s(modelFileName, "./data/cube.txt");
	
	strcpy_s(shaderPixelEntryPoint, PixelEntryPointToChar(SpecularMapPixelShader));
	strcpy_s(shaderVertexEntryPoint, VertexEntryPointToChar(TextureVertexShader));
	
	m_TextureShader = new ShaderClass;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), a_windowHandle, shaderVertexEntryPoint , shaderPixelEntryPoint);
    if (!result)
    {
	    MessageBox(a_windowHandle, L"Could not initialize texture shader object", L"Error", MB_OK);
    	return false;
    }

	
	strcpy_s(shaderPixelEntryPoint, PixelEntryPointToChar(FontPixelShader));
	m_FontShader = new ShaderClass;
	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), a_windowHandle, shaderVertexEntryPoint , shaderPixelEntryPoint);
	
    if (!result)
    {
	    MessageBox(a_windowHandle, L"Could not initialize texture shader object", L"Error", MB_OK);
    	return false;
    }
	
    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFileName, modelFileName, blendTexture1FileName, blendTexture2FileName, blendTexture3FileName, blendTexture4FileName);
    if (!result)
    {
        MessageBox(a_windowHandle, L"Could not initialize model object", L"Error", MB_OK);
        return false;
    }

	m_Font = new FontClass;
	result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
	if (!result)
	{
        MessageBox(a_windowHandle, L"Could not init font", L"Error", MB_OK);
        return false;
	}

	m_previousFps = -1;
	char fpsString[32];
	strcpy_s(fpsString, "Fps: 0");
	m_fpsText = new TextClass;
	if (!m_fpsText->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), a_screenWidth, a_screenHeight, 128, m_Font, fpsString, 10, 10, 1.0f,1.0f,1.0f))
		return false;
	
	m_MainLight = new LightClass;
	m_MainLight->m_LightDirection = XMFLOAT3(0.0f,-0.4f,1.0f);
	m_MainLight->m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_MainLight->m_SpecularColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_MainLight->m_SpecularPower = 16.0f;
	m_MainLight->m_AmbientColor = XMFLOAT4(0.1f,0.1f,0.1f,1.0f);

	//Pointlight variables for now to display multi lighting.
	m_LightDiffuse = new XMFLOAT4[4];
	m_LightPositions = new XMFLOAT4[4];
	m_LightDiffuse[0] = XMFLOAT4(1.0f,0.0f,0.0f,1.0f);
	m_LightPositions[0] = XMFLOAT4(-3.0f, 1.0f, 3.0f,1.0f);
	m_LightDiffuse[1] = XMFLOAT4(0.0f,1.0f,0.0f,1.0f);
	m_LightPositions[1] = XMFLOAT4(3.0f, 1.0f, 3.0f,1.0f);
	m_LightDiffuse[2] = XMFLOAT4(0.0f,0.0f,1.0f,1.0f);
	m_LightPositions[2] = XMFLOAT4(-3.0f,1.0f, -3.0f, 1.0f);
	m_LightDiffuse[3] = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_LightPositions[3] = XMFLOAT4(3.0f,1.0f, -3.0f, 1.0f);
	
    return true;
}

void ApplicationClass::Shutdown()
{
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = nullptr;
    }
	
	if (m_MainLight)
	{
		delete m_MainLight;
		m_MainLight = nullptr;
	}
	
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}
	
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_TextureShader;
		m_FontShader = nullptr;
	}
	
	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}
	
	if (m_Font)
	{
		m_Font->ShutDown();
		delete m_Font;
		m_Font = 0;
	}
	
	if (m_fpsText)
	{
		m_fpsText->Shutdown();
		delete m_fpsText;
		m_fpsText = 0;
	}
}

bool ApplicationClass::Frame(InputClass* a_InputClass)
{
	if (a_InputClass->IsEscapePressed())
		return false;

	static float rotation = 360.0f;
	
	// Update the rotation variable each frame.
    rotation -= 0.0174532925f * 0.25f;
    if(rotation <= 0.0f)
    {
        rotation += 360.0f;
    }

	UpdateFps();
	
	//Render Scene
    return Render(rotation);
}

bool ApplicationClass::Render(float a_Rotation) const
{
	XMMATRIX world;
	XMMATRIX worldRotation;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX ortho;
	
	constexpr XMFLOAT2 translationAmount = XMFLOAT2(0,0);
	constexpr float blendAmount = 0;;
	constexpr float waterTranslation = 0;
	constexpr float reflectRefractScale = 0;
	constexpr XMFLOAT4 pixelColor = XMFLOAT4(1,1,1,1);
	
    //clear buffers to begin the scene
    m_Direct3D->BeginScene(0.0f,0.0f,0.0f,1.0f);

    //update cameras view matrix
    m_Camera->Render();

    //get all the matrices
	m_Direct3D->GetWorldMatrix(world);
	m_Direct3D->GetWorldMatrix(worldRotation);
	m_Camera->GetViewMatrix(view);
	m_Direct3D->GetProjectionMatrix(projection);

	worldRotation = XMMatrixRotationY(a_Rotation);

	m_Direct3D->GetOrthoMatrix(ortho);
	//put the model vertex and index buffers into the graphics pipeline to prepare them to be drawn
	m_Model->Render(m_Direct3D->GetDeviceContext());

	

	bool result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
	                                            m_Model->GetIndexCount(),
	                                            worldRotation,
	                                            view,
	                                            projection,
	                                            m_Camera->GetPosition(),
	                                            0,
	                                            0,
	                                            XMFLOAT4(0,0,0,0),
	                                            world,
	                                            m_LightPositions,
	                                            m_MainLight->m_DiffuseColor,
	                                            m_MainLight->m_SpecularColor,
	                                            m_MainLight->m_SpecularPower,
	                                            m_MainLight->m_LightDirection,
	                                            m_MainLight->m_AmbientColor,
	                                            translationAmount,
	                                            blendAmount,
	                                            waterTranslation,
	                                            reflectRefractScale,
	                                            pixelColor,
	                                            m_LightDiffuse,
	                                            m_Model->GetTexture(0),
	                                            m_Model->GetTexture(1),
	                                            m_Model->GetTexture(2),
	                                            m_Model->GetTexture(3),
	                                            m_Model->GetTexture(4));
	if (!result)
	{
		return false;
	}

	m_Direct3D->GetWorldMatrix(world);
	m_Camera->GetViewMatrix(view);
	//2D SECTION-----------------------------------------------------------
	m_Direct3D->TurnZBufferOff();


	//UI SECTION
	m_Direct3D->EnableAlphaBlending();
	
	m_fpsText->Render(m_Direct3D->GetDeviceContext());
	result = m_FontShader->Render(m_Direct3D->GetDeviceContext(),
												m_Model->GetIndexCount(),
												world,
												view,
												ortho,
												m_Camera->GetPosition(),
												0,
												0,
												XMFLOAT4(0,0,0,0),
												world,
												m_LightPositions,
												m_MainLight->m_DiffuseColor,
												m_MainLight->m_SpecularColor,
												m_MainLight->m_SpecularPower,
												m_MainLight->m_LightDirection,
												m_MainLight->m_AmbientColor,
												translationAmount,
												blendAmount,
												waterTranslation,
												reflectRefractScale,
												pixelColor,
												m_LightDiffuse,
												m_Model->GetTexture(0),
												m_Model->GetTexture(1),
												m_Model->GetTexture(2),
												m_Model->GetTexture(3),
												m_Model->GetTexture(4));

	if (!result)
	{
		return false;
	}
	m_Direct3D->DisableAlphaBlending();
	m_Direct3D->TurnZBufferOn();
	//END 2D SECTION-------------------------------------------------------
    m_Direct3D->EndScene();
    return true;
}

bool ApplicationClass::UpdateFps()
{
	char tempString[16], finalString[16];
	float red, green, blue;
	
	m_count++;
    	
	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;
		m_startTime = timeGetTime();
	}

	int fps = m_fps;
	if (m_previousFps == fps)
	{
		return true;
	}
	m_previousFps = fps;
	
	if (fps > 99999)
	{
		fps = 99999;
	}
	sprintf_s(tempString, "%d", fps);
	strcpy_s(finalString, "Fps: ");
	strcat_s(finalString, tempString);
	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}
	else if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}
	else if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}
	else
	{
		red = 1.0f;
		green = 1.0f;
		blue = 1.0f;
	}
	
	const bool result = m_fpsText->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 50, 10, red, green, blue);
	if (!result)
		return false;

	return true;
}

const char* ApplicationClass::PixelEntryPointToChar(PixelShaderEntryPoint a_entryPoint)
{
	switch (a_entryPoint)
	{
		case SimpleLightingPixelShader:
			return "SimpleLightingPixelShader";
		case NormalMapPixelShader:
			return "NormalMapPixelShader";
		case SpecularMapPixelShader:
			return "SpecularMapPixelShader";
		case TextureSamplePixelShader:
			return "TextureSamplePixelShader";
		case TextureMultiLightPixelShader:
			return "TextureMultiLightPixelShader";
		case TextureMultiSamplePixelShader:
			return "TextureMultiSamplePixelShader";
		case TextureLightMapPixelShader:
			return "TextureLightMapPixelShader";
		case TextureAlphaMapPixelShader:
			return "TextureAlphaMapPixelShader";
		case FontPixelShader:
			return "FontPixelShader";
		default:
			return "SimpleLightingPixelShader";
	}
}

const char* ApplicationClass::VertexEntryPointToChar(VertexShaderEntryPoint a_entryPoint)
{
	switch (a_entryPoint)
	{
		case TextureVertexShader:
			return "TextureVertexShader";
		default:
			return "SimpleLightingPixelShader";
	}
}
