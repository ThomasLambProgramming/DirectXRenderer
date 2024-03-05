#include "ApplicationClass.h"

#include <iostream>

#include "Shader.h"

ApplicationClass* ApplicationClass::Instance = nullptr;

ApplicationClass::ApplicationClass()
{
	//Safety setting to 0/nullptr;
	Instance = this;
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_windowHandle = 0;
	m_lights = 0;
	m_basicLighting = 0;
}
ApplicationClass::~ApplicationClass() {}

bool ApplicationClass::Initialize(const int a_screenWidth, const int a_screenHeight, const HWND a_windowHandle)
{
	m_windowHandle = a_windowHandle;
    m_Direct3D = new DirectXApp();
	m_time = new ApplicationTime();
	m_time->Initialize();
    bool result = m_Direct3D->Initialize(a_screenWidth, a_screenHeight, VSYNC_ENABLED, a_windowHandle, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(a_windowHandle, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

	//ImguiInitialize();
	
	m_Camera = new Camera;
	m_Camera->SetPosition(0.0f,0.0f,-10.0f);
	
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
	m_shaders.push_back(new Shader(L"./src/shaders/textureVS.hlsl", L"./src/shaders/texturePS.hlsl", "TextureVertexShader", "SpecularMapPixelShader"));

	
	//Light information buffer 
	if (!m_shaders[0]->CreateDynamicBuffer(sizeof(LightInformationBufferType), false))
		return false;
	//Matrix buffer
	if (!m_shaders[0]->CreateDynamicBuffer(sizeof(MatrixBufferType), true))
		return false;
	//Camera Buffer
	if (!m_shaders[0]->CreateDynamicBuffer(sizeof(CameraBufferType), true))
		return false;
	
	return true;
}

bool ApplicationClass::SetupModels()
{
	char textureFileName[128];
	char blendTexture1FileName[128];
	char blendTexture2FileName[128];

	m_objects = vector<GameObject*>();
	strcpy_s(textureFileName, "./data/stone02.tga");
	strcpy_s(blendTexture1FileName, "./data/normal02.tga");
	strcpy_s(blendTexture2FileName, "./data/spec02.tga");
	
	char* textureFileNames[] = {
		textureFileName,
		blendTexture1FileName,
		blendTexture2FileName,
	};

	m_objects.push_back(new GameObject());
	
	bool result = m_objects[0]->InitializePrimitive(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), GameObject::Cube, textureFileNames, 3);
	if (!result)
	{
		MessageBox(m_windowHandle, L"Could not initialize model object", L"Error", MB_OK);
		return false;
	}
	return true;
}

void ApplicationClass::InitializeLights()
{
	//first light is main directional light and the others are point lights
	m_lights = new Light[NUM_LIGHTS];
	m_lights[0].m_LightDirection = XMFLOAT3(0.0f,-0.4f,1.0f);
	m_lights[0].m_Position = XMFLOAT4(0.0f,0.0f,0.0f,0.0f);
	m_lights[0].m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_lights[0].m_SpecularColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	m_lights[0].m_SpecularPower = 16.0f;
	m_lights[0].m_AmbientColor = XMFLOAT4(0.1f,0.1f,0.1f,1.0f);
	m_lights[1].m_DiffuseColor = XMFLOAT4(1.0f,0.0f,0.0f,1.0f);
	m_lights[1].m_Position = XMFLOAT4(-3.0f, 1.0f, 3.0f,1.0f);
	m_lights[2].m_DiffuseColor = XMFLOAT4(0.0f,1.0f,0.0f,1.0f);
	m_lights[2].m_Position = XMFLOAT4(3.0f, 1.0f, 3.0f,1.0f);
	m_lights[3].m_DiffuseColor = XMFLOAT4(0.0f,0.0f,1.0f,1.0f);
	m_lights[3].m_Position = XMFLOAT4(-3.0f,1.0f, -3.0f, 1.0f);

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		m_lights[i].m_DiffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
		m_lights[i].m_SpecularColor = XMFLOAT4(1.0f,1.0f,0.0f,1.0f);
		m_lights[i].m_AmbientColor = XMFLOAT4(0.7f,0.7f,0.7f,0.7f);
		if (i > 0)
			m_lights[i].m_LightDirection = XMFLOAT3(1.0f,1.0f,1.0f);
		m_lights[i].m_SpecularPower = 1.0f;
	}
}

void ApplicationClass::Shutdown()
{
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = nullptr;
    }
	if (m_lights)
	{
		delete[] m_lights;
		m_lights = nullptr;
	}
	if (m_basicLighting)
	{
		delete m_basicLighting;
		m_basicLighting = nullptr;
	}
	while (m_objects.size() > 0)
	{
		delete m_objects.back();
		m_objects.pop_back();
	}
	while (m_shaders.size() > 0)
	{
		delete m_shaders.back();
		m_shaders.pop_back();
	}
	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}
	if (m_time)
	{
		delete m_time;
		m_time = nullptr;
	}
	//ImGui_ImplDX11_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //ImGui::DestroyContext();
}

bool ApplicationClass::Frame(InputManager* a_InputClass) 
{
	m_time->Frame();
	if (GetForegroundWindow() != m_windowHandle)
	{
		return true;
	}
	if (a_InputClass->IsEscapePressed())
		return false;
	
	XMFLOAT2 camInput = a_InputClass->GetWasdValue();
	XMFLOAT3 cameraPosition = m_Camera->GetPosition();
	cameraPosition.x += camInput.x * m_time->GetDeltaTime();
	cameraPosition.z += camInput.y * m_time->GetDeltaTime();

	XMVECTOR cameraPositionVector = XMLoadFloat3(&cameraPosition);
	cameraPositionVector += XMLoadFloat3(&m_Camera->m_rightVector) * camInput.x * 0.1f;
	cameraPositionVector += XMLoadFloat3(&m_Camera->m_forwardVector) * camInput.y * 0.1f;
	XMStoreFloat3(&cameraPosition, cameraPositionVector);
	camInput.y = 0;
	if (a_InputClass->IsSpacePressed())
		camInput.y += 1;
	if (a_InputClass->IsShiftPressed())
		camInput.y -= 1;
	cameraPosition.y += camInput.y * m_time->GetDeltaTime() * 5;
	m_Camera->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	RECT rect;
    GetClientRect(m_windowHandle, &rect);

    POINT ul;
    ul.x = rect.left;
    ul.y = rect.top;

    POINT lr;
    lr.x = rect.right;
    lr.y = rect.bottom;

    MapWindowPoints(m_windowHandle, nullptr, &ul, 1);
    MapWindowPoints(m_windowHandle, nullptr, &lr, 1);

    rect.left = ul.x;
    rect.top = ul.y;

    rect.right = lr.x;
    rect.bottom = lr.y;

	XMFLOAT2 middlePosition = XMFLOAT2((rect.right - rect.left) / 2 + rect.left, (rect.bottom - rect.top) / 2 + rect.top);

	XMFLOAT2 mouseDelta = XMFLOAT2(m_currentMousePos.x - (middlePosition.x - rect.left), m_currentMousePos.y - (middlePosition.y - rect.top));

	if (m_currentMousePos.x == 0 && m_currentMousePos.y == 0)
	{
		mouseDelta.y = 0;
		mouseDelta.x = 0;
	}
	XMFLOAT3 cameraRotation = m_Camera->GetRotation();
	cameraRotation.y += mouseDelta.x * m_time->GetDeltaTime() * 5;
	cameraRotation.x += mouseDelta.y * m_time->GetDeltaTime() * 5;
	
	m_Camera->SetRotation(cameraRotation.x, cameraRotation.y, cameraRotation.z);
	m_prevMousePos = m_currentMousePos;
    rotation -= 0.0174532925f * 0.25f;
    if(rotation <= 0.0f)
    {
        rotation += 360.0f;
    }
    ClipCursor(&rect);

	SetCursorPos(middlePosition.x, middlePosition.y);
	//Render Scene
    return Render();
}

bool ApplicationClass::Render() 
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX worldRotation;
	
	//clear buffers to begin the scene
	//the above makes the fog work but as we dont need it right now it is not used.
	//m_Direct3D->BeginScene(fogColor, fogColor, fogColor,1.0f);
	//This is a number that was taken from a rgba color picker.
	m_Direct3D->BeginScene(216.0f / 255.0f,148.0f /255.0f,120.0f /255, 1);

	//get all the matrices
	m_Direct3D->GetWorldMatrix(world);
	m_Direct3D->GetWorldMatrix(worldRotation);
	worldRotation = XMMatrixRotationY(rotation);
	
	m_Camera->GetViewMatrix(view);
	m_Direct3D->GetProjectionMatrix(projection);
	MatrixBufferType* matrixData = new MatrixBufferType;
	matrixData->world = worldRotation;
	matrixData->view = view;
	matrixData->projection = projection;
	
	CameraBufferType* cameraData = new CameraBufferType;
	cameraData->padding = 0;
	cameraData->cameraPos = m_Camera->GetPosition();

	LightInformationBufferType* lightData = new LightInformationBufferType;
	lightData->lightDiffuse = m_lights[0].m_DiffuseColor;
	lightData->lightSpecularPower = m_lights[0].m_SpecularPower;
	lightData->lightDirection = m_lights[0].m_LightDirection;
	matrixData->world = XMMatrixTranspose(matrixData->world);
	matrixData->view = XMMatrixTranspose(matrixData->view);
	matrixData->projection = XMMatrixTranspose(matrixData->projection);

	//RENDERING 3D SHIT HERE!
	m_shaders[0]->SetBufferData(0, matrixData, sizeof(MatrixBufferType), true);
	m_shaders[0]->SetBufferData(1, cameraData, sizeof(CameraBufferType), true);
	m_shaders[0]->SetBufferData(0, lightData, sizeof(LightInformationBufferType), false);
	
	
	m_objects[0]->SetAsObjectToRender(m_Direct3D->GetDeviceContext());
	for (int j = 0; j < m_objects[0]->GetTextureCount(); j++)
		m_shaders[0]->SetShaderResources(j, m_objects[0]->GetTexture(j));
	m_shaders[0]->RenderShader(m_objects[0]->GetIndexCount());
	
	delete matrixData;
	matrixData = nullptr;
	delete lightData;
	lightData = nullptr;
	delete cameraData;
	cameraData = nullptr;

	
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