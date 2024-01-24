#include "ApplicationClass.h"

#include <complex>

ApplicationClass::ApplicationClass(): m_FontShader(nullptr),
                                      m_Font(nullptr),
                                      m_fpsText(nullptr)
{
	//Safety setting to 0/nullptr;
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_ModelShader = nullptr;
	m_MainLight = nullptr;
	m_SpriteShader = nullptr;
	m_startTime = 0;
	m_previousFps = 0;
	m_fps = 0;
	m_count = 0;
	m_ObjectPosX = new float;
	m_ObjectPosY = new float;
	*m_ObjectPosY = 1.0f;
	*m_ObjectPosX = 1.0f;
}

ApplicationClass::ApplicationClass(const ApplicationClass&): m_Direct3D(nullptr), m_Camera(nullptr), 
                                                             m_MainLight(nullptr),
                                                             m_ModelShader(nullptr), m_FontShader(nullptr),
                                                             m_SpriteShader(nullptr),
                                                             m_fps(0),
                                                             m_count(0),
                                                             m_startTime(0), m_previousFps(0), m_Font(nullptr),
                                                             m_fpsText(nullptr),
                                                             m_ObjectPosX(nullptr),
                                                             m_ObjectPosY(nullptr)
{
}

ApplicationClass::~ApplicationClass() {}

bool ApplicationClass::Initialize(const int a_screenWidth, const int a_screenHeight, const HWND a_windowHandle)
{
	m_startTime = timeGetTime();
    m_Direct3D = new Direct3DClass;

    bool result = m_Direct3D->Initialize(a_screenWidth, a_screenHeight, VSYNC_ENABLED, a_windowHandle, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(a_windowHandle, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

	ImguiInitialize(a_windowHandle);
    CameraInitialize();
	
	if (!InitializeShaders(a_windowHandle))
		return true;

	result = SetupModels(a_screenWidth, a_screenHeight, a_windowHandle);
	if (!result)
	{
        MessageBox(a_windowHandle, L"Failed to init models", L"Error", MB_OK);
        return false;
	}

	result = InitializeFontAndText(a_screenWidth, a_screenHeight, a_windowHandle);
	if (!result)
	{
        MessageBox(a_windowHandle, L"Failed to init Font/Text", L"Error", MB_OK);
        return false;
	}

	//Setup basic light data structures with default values.
	InitializeLights();

    return true;
}

void ApplicationClass::ImguiInitialize(const HWND a_windowHandle) const
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(a_windowHandle);
	ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
}

bool ApplicationClass::InitializeShaders(const HWND a_windowHandle)
{
	char shaderVertexEntryPoint[128];
	char shaderPixelEntryPoint[128];
	strcpy_s(shaderPixelEntryPoint, PixelEntryPointToChar(SpecularMapPixelShader));
	strcpy_s(shaderVertexEntryPoint, VertexEntryPointToChar(TextureVertexShader));
	
	m_ModelShader = new ShaderClass;
	bool result = m_ModelShader->Initialize(m_Direct3D->GetDevice(), a_windowHandle, shaderVertexEntryPoint , shaderPixelEntryPoint);
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
	
	strcpy_s(shaderPixelEntryPoint, PixelEntryPointToChar(TransparentColorPixelShader));
	m_SpriteShader = new ShaderClass;
	result = m_SpriteShader->Initialize(m_Direct3D->GetDevice(), a_windowHandle, shaderVertexEntryPoint , shaderPixelEntryPoint);
	
	if (!result)
	{
		MessageBox(a_windowHandle, L"Could not initialize texture shader object", L"Error", MB_OK);
		return false;
	}
	return true;
}


void ApplicationClass::CameraInitialize()
{
	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f,0.0f,-10.0f);
	//m_Camera->SetRotation(20.0f,0.0f,0.0f);
}

bool ApplicationClass::SetupModels(const int a_screenWidth, const int a_screenHeight, const HWND a_windowHandle)
{
	char textureFileName[128];
	char blendTexture1FileName[128];
	char blendTexture2FileName[128];
	char blendTexture3FileName[128];
	char blendTexture4FileName[128];

	m_3DObjects = vector<ObjectClass*>();
	strcpy_s(textureFileName, "./data/stone02.tga");
	strcpy_s(blendTexture1FileName, "./data/normal02.tga");
	strcpy_s(blendTexture2FileName, "./data/spec02.tga");
	strcpy_s(blendTexture3FileName, "./data/alpha01.tga");
	strcpy_s(blendTexture4FileName, "./data/font01.tga");
	
	char* textureFileNames[] = {
		textureFileName,
		blendTexture1FileName,
		blendTexture2FileName,
		blendTexture3FileName,
		blendTexture4FileName,
		blendTexture2FileName
	};

	m_3DObjects.push_back(new ObjectClass());
	m_2DObjects.push_back(new ObjectClass());
	
	bool result = m_3DObjects[0]->InitializePrimitive(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), ObjectClass::Sphere, textureFileNames, 6);
	if (!result)
	{
		MessageBox(a_windowHandle, L"Could not initialize model object", L"Error", MB_OK);
		return false;
	}
	
	result = m_2DObjects[0]->Initialize2DQuad(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), a_screenWidth, a_screenHeight, 300, 0, textureFileNames, 6);
	if (!result)
	{
		MessageBox(a_windowHandle, L"Could not initialize model object", L"Error", MB_OK);
		return false;
	}
	return true;
}

void ApplicationClass::InitializeLights()
{
	m_MainLight = new LightClass;
	m_MainLight->m_LightDirection = XMFLOAT3(0.0f,-0.4f,1.0f);
	m_MainLight->m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_MainLight->m_SpecularColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_MainLight->m_SpecularPower = 16.0f;
	m_MainLight->m_AmbientColor = XMFLOAT4(0.1f,0.1f,0.1f,1.0f);

	//Pointlight variables for now to display multi lighting.
	m_PointLights = new LightClass[NUM_LIGHTS];
	m_PointLights[0].m_DiffuseColor = XMFLOAT4(1.0f,0.0f,0.0f,1.0f);
	m_PointLights[0].m_Position = XMFLOAT4(-3.0f, 1.0f, 3.0f,1.0f);
	m_PointLights[1].m_DiffuseColor = XMFLOAT4(0.0f,1.0f,0.0f,1.0f);
	m_PointLights[1].m_Position = XMFLOAT4(3.0f, 1.0f, 3.0f,1.0f);
	m_PointLights[2].m_DiffuseColor = XMFLOAT4(0.0f,0.0f,1.0f,1.0f);
	m_PointLights[2].m_Position = XMFLOAT4(-3.0f,1.0f, -3.0f, 1.0f);
	m_PointLights[3].m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_PointLights[3].m_Position = XMFLOAT4(3.0f,1.0f, -3.0f, 1.0f);
}

bool ApplicationClass::InitializeFontAndText(const int a_screenWidth, const int a_screenHeight, const HWND a_windowHandle)
{
	bool result;
	m_Font = new FontClass;
	result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
	if (!result)
	{
		MessageBox(a_windowHandle, L"Could not init font", L"Error", MB_OK);
		return false;
	}
	m_previousFps = -1;
	
	m_UIObjects = vector<TextClass*>();
	m_UIObjects.push_back(new TextClass());
	char fpsString[32];
	strcpy_s(fpsString, "Fps: 0");
	if (!m_UIObjects[0]->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), a_screenWidth, a_screenHeight, 128, m_Font, fpsString, 10, 10, 1.0f,1.0f,1.0f))
		return false;
	
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
	
	if (m_ModelShader)
	{
		m_ModelShader->Shutdown();
		delete m_ModelShader;
		m_ModelShader = nullptr;
	}
	
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_ModelShader;
		m_FontShader = nullptr;
	}
	
	for (int i = 0; i < m_2DObjects.size(); i++)
	{
		m_2DObjects[i]->Shutdown();
	}
	m_2DObjects.clear();
	
	for (int i = 0; i < m_3DObjects.size(); i++)
	{
		m_3DObjects[i]->Shutdown();
	}
	m_3DObjects.clear();
	
	for (int i = 0; i < m_UIObjects.size(); i++)
	{
		m_UIObjects[i]->Shutdown();
	}
	m_UIObjects.clear();

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
		m_Font = nullptr;
	}
	
	if (m_SpriteShader)
	{
		m_SpriteShader->Shutdown();
		delete m_SpriteShader;
		m_SpriteShader = nullptr;
	}

	ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
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
	constexpr float blendAmount = 0.1f;
	constexpr float waterTranslation = 0;
	constexpr float reflectRefractScale = 0;
	constexpr XMFLOAT4 pixelColor = XMFLOAT4(1,1,1,1);
	
	//clear buffers to begin the scene
	m_Direct3D->BeginScene(0.0f,0.0f,0.0f,1.0f);

	XMFLOAT4 lightPositions[] = {
		m_PointLights[0].m_Position,
		m_PointLights[1].m_Position,
		m_PointLights[2].m_Position,
		m_PointLights[3].m_Position
	};

	XMFLOAT4 lightDiffuse[] = {
		m_PointLights[0].m_DiffuseColor,
		m_PointLights[1].m_DiffuseColor,
		m_PointLights[2].m_DiffuseColor,
		m_PointLights[3].m_DiffuseColor
	};
 
	
	//update cameras view matrix
	m_Camera->Render();

	//get all the matrices
	m_Direct3D->GetWorldMatrix(world);
	m_Direct3D->GetWorldMatrix(worldRotation);
	m_Camera->GetViewMatrix(view);
	m_Direct3D->GetProjectionMatrix(projection);
	worldRotation = XMMatrixRotationY(a_Rotation);
	m_Direct3D->GetOrthoMatrix(ortho);

	for (int i = 0; i < m_3DObjects.size(); i++)
	{
		m_3DObjects[i]->SetAsObjectToRender(m_Direct3D->GetDeviceContext());
		const bool result = m_ModelShader->Render(m_Direct3D->GetDeviceContext(),
		                                          m_3DObjects[i]->GetIndexCount(),
		                                          worldRotation,
		                                          view,
		                                          projection,
		                                          m_Camera->GetPosition(),
		                                          0,
		                                          0,
		                                          XMFLOAT4(0,0,0,0),
		                                          world,
		                                          lightPositions,
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
		                                          lightDiffuse,
		                                          m_3DObjects[i]->GetTexture(0),
		                                          m_3DObjects[i]->GetTexture(1),
		                                          m_3DObjects[i]->GetTexture(2),
		                                          m_3DObjects[i]->GetTexture(3),
		                                          m_3DObjects[i]->GetTexture(4),
		                                          m_3DObjects[i]->GetTextureCount());
		if (!result)
		{
			return false;
		}
	}

	


	//2D SECTION-----------------------------------------------------------
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();
	for (int i = 0; i < m_2DObjects.size(); i++)
	{
		m_2DObjects[i]->Update2DBuffers(m_Direct3D->GetDeviceContext());
		m_2DObjects[i]->SetAsObjectToRender(m_Direct3D->GetDeviceContext());

		const bool result = m_SpriteShader->Render(m_Direct3D->GetDeviceContext(),
		                                          m_2DObjects[i]->GetIndexCount(),
		                                          world,
		                                          view,
		                                          ortho,
		                                          m_Camera->GetPosition(),
		                                          0,
		                                          0,
		                                          XMFLOAT4(0,0,0,0),
		                                          world,
		                                          lightPositions,
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
		                                          lightDiffuse,
		                                          m_2DObjects[i]->GetTexture(0),
		                                          m_2DObjects[i]->GetTexture(1),
		                                          m_2DObjects[i]->GetTexture(2),
		                                          m_2DObjects[i]->GetTexture(3),
		                                          m_2DObjects[i]->GetTexture(4),
		                                          m_2DObjects[i]->GetTextureCount());
		if (!result)
		{
			return false;
		}
	}

	//UI SECTION
	for (int i = 0; i < m_UIObjects.size(); i++)
	{
		m_UIObjects[i]->Render(m_Direct3D->GetDeviceContext());

		const bool result = m_FontShader->Render(m_Direct3D->GetDeviceContext(),
		                                          m_UIObjects[i]->GetIndexCount(),
		                                          world,
		                                          view,
		                                          ortho,
		                                          m_Camera->GetPosition(),
		                                          0,
		                                          0,
		                                          XMFLOAT4(0,0,0,0),
		                                          world,
		                                          lightPositions,
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
		                                          lightDiffuse,
		                                          m_Font->GetTexture(),
		                                          m_Font->GetTexture(),
		                                          m_Font->GetTexture(),
		                                          m_Font->GetTexture(),
		                                          m_Font->GetTexture(),
		                                          5);
		if (!result)
		{
			return false;
		}
	}

	
	m_Direct3D->DisableAlphaBlending();
	m_Direct3D->TurnZBufferOn();
	//END 2D SECTION-------------------------------------------------------


	//Imgui Section-------------------------------------------------------
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	XMFLOAT3 position;
	ImGui::Begin("Window");
	//if (ImGui::SliderFloat("XPosition", m_ObjectPosX, 0.0f, 800.0f))
	//{
	//	position = m_Model->GetPosition();
	//	position.x = *m_ObjectPosX;
	//	m_Model->SetPosition(position);	
	//}
	//if (ImGui::SliderFloat("YPosition", m_ObjectPosY, 0.0f, 800.0f))
	//{
	//	position = m_Model->GetPosition();
	//	position.y = *m_ObjectPosY;
	//	m_Model->SetPosition(position);	
	//}
	//Do Imgui here.
	ImGui::End();
	ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//END Imgui Section-------------------------------------------------------

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
	
	const bool result = m_UIObjects[0]->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 50, 10, red, green, blue);
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
		case TransparentColorPixelShader:
			return "TransparentColorPixelShader";
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
