#include "ApplicationClass.h"
#include "Shader.h"

ApplicationClass* ApplicationClass::Instance = nullptr;

ApplicationClass::ApplicationClass()
{
	Instance = this;
	//Safety setting to 0/nullptr;
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_mainDirectionalLight = nullptr;
}
ApplicationClass::~ApplicationClass() {}

bool ApplicationClass::Initialize(const int a_screenWidth, const int a_screenHeight, const HWND a_windowHandle)
{
	m_windowHandle = a_windowHandle;
    m_Direct3D = new DirectXApp;

    bool result = m_Direct3D->Initialize(a_screenWidth, a_screenHeight, VSYNC_ENABLED, a_windowHandle, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(a_windowHandle, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

	ImguiInitialize();
	
	m_Camera = new Camera;
	m_Camera->SetPosition(0.0f,0.0f,-10.0f);
	//m_Camera->SetRotation(20.0f,0.0f,0.0f);
	
	if (!InitializeShaders())
		return true;

	result = SetupModels();
	if (!result)
	{
        MessageBox(a_windowHandle, L"Failed to init models", L"Error", MB_OK);
        return false;
	}

	//Setup basic light data structures with default values.
	InitializeLights();

    return true;
}

void ApplicationClass::ImguiInitialize() const
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_windowHandle);
	ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
}

bool ApplicationClass::InitializeShaders()
{
	char shaderVertexEntryPoint[128];
	char shaderPixelEntryPoint[128];
	strcpy_s(shaderPixelEntryPoint, "");
	strcpy_s(shaderVertexEntryPoint, "");
	return true;
}

bool ApplicationClass::SetupModels()
{
	char textureFileName[128];
	char blendTexture1FileName[128];
	char blendTexture2FileName[128];
	char blendTexture3FileName[128];
	char blendTexture4FileName[128];

	m_objects = vector<GameObject*>();
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

	m_objects.push_back(new GameObject());
	
	bool result = m_objects[0]->InitializePrimitive(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), GameObject::Sphere, textureFileNames, 6);
	if (!result)
	{
		MessageBox(m_windowHandle, L"Could not initialize model object", L"Error", MB_OK);
		return false;
	}
	return true;
}

void ApplicationClass::InitializeLights()
{
	m_mainDirectionalLight = new Light;
	m_mainDirectionalLight->m_LightDirection = XMFLOAT3(0.0f,-0.4f,1.0f);
	m_mainDirectionalLight->m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_mainDirectionalLight->m_SpecularColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_mainDirectionalLight->m_SpecularPower = 16.0f;
	m_mainDirectionalLight->m_AmbientColor = XMFLOAT4(0.1f,0.1f,0.1f,1.0f);

	//Pointlight variables for now to display multi lighting.
	m_PointLights = new Light[NUM_LIGHTS];
	m_PointLights[0].m_DiffuseColor = XMFLOAT4(1.0f,0.0f,0.0f,1.0f);
	m_PointLights[0].m_Position = XMFLOAT4(-3.0f, 1.0f, 3.0f,1.0f);
	m_PointLights[1].m_DiffuseColor = XMFLOAT4(0.0f,1.0f,0.0f,1.0f);
	m_PointLights[1].m_Position = XMFLOAT4(3.0f, 1.0f, 3.0f,1.0f);
	m_PointLights[2].m_DiffuseColor = XMFLOAT4(0.0f,0.0f,1.0f,1.0f);
	m_PointLights[2].m_Position = XMFLOAT4(-3.0f,1.0f, -3.0f, 1.0f);
	m_PointLights[3].m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_PointLights[3].m_Position = XMFLOAT4(3.0f,1.0f, -3.0f, 1.0f);
}

void ApplicationClass::Shutdown()
{
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = nullptr;
    }
	
	if (m_mainDirectionalLight)
	{
		delete m_mainDirectionalLight;
		m_mainDirectionalLight = nullptr;
	}
	
	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}
	
	ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool ApplicationClass::Frame(InputManager* a_InputClass) const
{
	if (a_InputClass->IsEscapePressed())
		return false;

	//Render Scene
    return Render();
}

bool ApplicationClass::Render() const
{
	XMMATRIX world;
	XMMATRIX worldRotation;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX ortho;
	
	//constexpr XMFLOAT2 translationAmount = XMFLOAT2(0,0.0f);
	//constexpr float blendAmount = 0.1f;
	//constexpr float waterTranslation = 0;
	//constexpr float reflectRefractScale = 0;
	//constexpr XMFLOAT4 pixelColor = XMFLOAT4(1,1,1,1);
	//constexpr float fogStart = 1000.0f;
	//constexpr float fogEnd = 10000.0f;
	//constexpr float fogColor = 0.5f;
	
	//clear buffers to begin the scene
	//the above makes the fog work but as we dont need it right now it is not used.
	//m_Direct3D->BeginScene(fogColor, fogColor, fogColor,1.0f);
	//This is a number that was taken from a rgba color picker.
	m_Direct3D->BeginScene(216.0f / 255.0f,148.0f /255.0f,120.0f /255, 1);

	//Clip plane works but just for now we dont have a use for it.
	//XMFLOAT4 clipPlane = XMFLOAT4(0.0f,0.0f,0.0f,0.0f);
 
	
	//update cameras view matrix
	m_Camera->Render();

	//get all the matrices
	m_Direct3D->GetWorldMatrix(world);
	m_Direct3D->GetWorldMatrix(worldRotation);
	m_Camera->GetViewMatrix(view);
	m_Direct3D->GetProjectionMatrix(projection);
	//worldRotation = XMMatrixRotationY(a_Rotation);
	m_Direct3D->GetOrthoMatrix(ortho);


	//RENDERING 3D SHIT HERE!



	
	//2D SECTION-----------------------------------------------------------
	//m_Direct3D->TurnZBufferOff();
	//m_Direct3D->EnableAlphaBlending();

	//UI SECTION
	//m_Direct3D->DisableAlphaBlending();
	//m_Direct3D->TurnZBufferOn();
	//END 2D SECTION-------------------------------------------------------


	//Imgui Section-------------------------------------------------------
	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();
	//XMFLOAT3 position;
	//ImGui::Begin("Window");
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
	//ImGui::End();
	//ImGui::Render();
    //ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//END Imgui Section-------------------------------------------------------

    m_Direct3D->EndScene();
    return true;
}