// ReSharper disable CppClangTidyClangDiagnosticReservedMacroIdentifier
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include "Direct3DClass.h"
#include "CameraClass.h"
#include "ObjectClass.h"
#include "ShaderClass.h"
#include "InputClass.h"
#include "LightClass.h"

#include <Windows.h>
#include <mmsystem.h>

#include "thirdparty/imgui/imgui_impl_dx11.h"
#include "thirdparty/imgui/imgui_impl_win32.h"
#include "thirdparty/imgui/imgui.h"

#include "TextClass.h"
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
		FontPixelShader
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
	bool Frame(InputClass* a_InputClass);

private:
	bool Render(float a_Rotation) const;
	
	bool UpdateFps();
	static const char* PixelEntryPointToChar(PixelShaderEntryPoint a_entryPoint);
	static const char* VertexEntryPointToChar(VertexShaderEntryPoint a_entryPoint);

private:
	Direct3DClass* m_Direct3D;
	CameraClass* m_Camera;

	std::vector<ObjectClass*> m_3DObjects;
	std::vector<ObjectClass*> m_2DObjects;
	std::vector<ObjectClass*> m_UIObjects;
	
	LightClass* m_MainLight;

	std::vector<LightClass*> m_PointLights;
	
	ShaderClass* m_ModelShader;
	ShaderClass* m_FontShader;
	ShaderClass* m_SpriteShader;

	//Fps related variables.
	unsigned long m_startTime;
	int m_fps;
	int m_count;
	int m_previousFps;

	FontClass* m_Font;
	TextClass* m_fpsText;

	float* m_ObjectPosX;
	float* m_ObjectPosY;
};

#endif