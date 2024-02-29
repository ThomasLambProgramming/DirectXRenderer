#pragma once

#include "DirectXApp.h"
#include "Camera.h"
#include "GameObject.h"
#include "DEPRICATED_ShaderClass.h"
#include "InputManager.h"
#include "Light.h"

#include <Windows.h>
#include <mmsystem.h>

#include "thirdparty/imgui/imgui_impl_dx11.h"
#include "thirdparty/imgui/imgui_impl_win32.h"
#include "thirdparty/imgui/imgui.h"

#pragma comment(lib, "winmm.lib")

class FontShaderClass;

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
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);

	void CameraInitialize();
	bool SetupModels(int a_screenWidth, int a_screenHeight, HWND a_windowHandle);
	void InitializeLights();
	bool InitializeFontAndText(int a_screenWidth, int a_screenHeight, HWND a_windowHandle);
	void ImguiInitialize(HWND a_windowHandle) const;
	bool InitializeShaders(HWND a_windowHandle);

	void Shutdown();
	bool Frame(InputManager* a_InputClass);

	//In large cleanup of code to make using this renderer much simpler and easier I am using a singleton for access to the Direct3DClass and etc so its not argument tunneling
	static ApplicationClass* Instance;

public:
	bool Render(float a_Rotation) const;
	
	bool UpdateFps();
	static const char* PixelEntryPointToChar(PixelShaderEntryPoint a_entryPoint);
	static const char* VertexEntryPointToChar(VertexShaderEntryPoint a_entryPoint);

	DirectXApp* m_Direct3D;
	Camera* m_Camera;

	HWND m_windowHandle;

	std::vector<GameObject*> m_3DObjects;
	std::vector<GameObject*> m_2DObjects;
	
	Light* m_MainLight;

	//This is an array that has 4 elements as per NUM_LIGHTS constant.
	Light* m_PointLights;
	
	DEPRICATED_ShaderClass* m_ModelShader;
	DEPRICATED_ShaderClass* m_FontShader;
	DEPRICATED_ShaderClass* m_SpriteShader;

	//Fps related variables.
	unsigned long m_startTime;
	int m_fps;
	int m_count;
	int m_previousFps;

	float* m_ObjectPosX;
	float* m_ObjectPosY;
};