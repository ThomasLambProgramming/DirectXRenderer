#pragma once
#define NUM_LIGHTS 4

#include "DirectXApp.h"
#include "Camera.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Light.h"
#include <complex>
#include <Windows.h>
#include <mmsystem.h>

#include "ApplicationTime.h"
#include "Shader.h"

#include "thirdparty/imgui/imgui_impl_dx11.h"
#include "thirdparty/imgui/imgui_impl_win32.h"
#include "thirdparty/imgui/imgui.h"

#pragma comment(lib, "winmm.lib")

class Shader;
//GLOBALS
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class ApplicationClass
{
	enum VertexShaderEntryPoint
	{
		TextureVertexShader = 0,
	};
	enum PixelShaderEntryPoint
	{
		SimpleLightingPixelShader = 0,
		NormalMapPixelShader = 1,
		SpecularMapPixelShader,
		TextureSamplePixelShader,
		TextureMultiLightPixelShader,
		TextureMultiSamplePixelShader,
		TextureLightMapPixelShader,
		TextureAlphaMapPixelShader,
		FontPixelShader,
		TransparentColorPixelShader,
		TextureSampleFogPixelShader,
	};
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct CameraBufferType 
	{
		XMFLOAT3 cameraPos;
		float padding;
	};
	struct LightInformationBufferType 
	{
		XMFLOAT4 lightDiffuse;
		XMFLOAT3 lightDirection;
		float lightSpecularPower;
	};
public:
	ApplicationClass();
	~ApplicationClass();
	ApplicationClass(const ApplicationClass&) = delete;
	ApplicationClass(ApplicationClass&&) = delete;
	ApplicationClass& operator=(const ApplicationClass&) = delete;
	ApplicationClass& operator=(ApplicationClass&&) = delete;

	bool Initialize(int, int, HWND);

	bool MakeObjects();
	void ImguiInitialize() const;
	bool InitializeShaders();
	void InitializeLights();

	void Shutdown();
	void ProcessMouseInput(InputManager* a_InputClass);
	bool Frame(InputManager* a_InputClass);
	bool Render();
	bool AddObject(char* textureFileNames[], int a_textureCount, XMFLOAT3 a_position, XMFLOAT3 a_rotation, GameObject::PrimitiveType a_primitive = GameObject::Cube);

	HWND m_windowHandle;
	DirectXApp* m_Direct3D;
	Camera* m_Camera;
	std::vector<GameObject*> m_objects;
	std::vector<Shader*> m_shaders;
	Light* m_lights;
	Shader* m_basicLighting;
	static ApplicationClass* Instance;
	ApplicationTime* m_time;
	XMFLOAT2 m_prevMousePos;
	XMFLOAT2 m_currentMousePos;
private:
	float rotation;
	//In large cleanup of code to make using this renderer much simpler and easier I am using a singleton for access to the Direct3DClass and etc so its not argument tunneling
};