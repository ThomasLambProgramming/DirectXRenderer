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
public:
	ApplicationClass();
	~ApplicationClass();
	ApplicationClass(const ApplicationClass&) = delete;
	ApplicationClass(ApplicationClass&&) = delete;
	ApplicationClass& operator=(const ApplicationClass&) = delete;
	ApplicationClass& operator=(ApplicationClass&&) = delete;

	bool Initialize(int, int, HWND);

	bool SetupModels();
	void ImguiInitialize() const;
	bool InitializeShaders();
	void InitializeLights();

	void Shutdown();
	bool Frame(InputManager* a_InputClass) const;
	bool Render() const;

	HWND m_windowHandle;
	DirectXApp* m_Direct3D;
	Camera* m_Camera;
	std::vector<GameObject*> m_objects;
	Light* m_mainDirectionalLight;
	Light* m_PointLights;
	Shader* m_basicLighting;
	
	//In large cleanup of code to make using this renderer much simpler and easier I am using a singleton for access to the Direct3DClass and etc so its not argument tunneling
	static ApplicationClass* Instance;
};