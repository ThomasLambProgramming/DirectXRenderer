// ReSharper disable CppClangTidyClangDiagnosticReservedMacroIdentifier
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include "Direct3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ShaderClass.h"
#include "InputClass.h"

#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class FontShaderClass;

//GLOBALS
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initalize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass* a_InputClass);

private:
	bool Render(float a_Rotation);
	bool UpdateFps();
	bool UpdateMouseStrings(int posX, int posY, bool a_MouseDown);

private:
	Direct3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	
	ShaderClass* m_TextureShader;
	
	int m_fps;
	int m_count;
	unsigned long m_startTime;
	int m_previousFps;
};

#endif