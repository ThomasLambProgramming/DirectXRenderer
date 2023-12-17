// ReSharper disable CppClangTidyClangDiagnosticReservedMacroIdentifier
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include "Direct3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "TextureShaderClass.h"
#include "LightClass.h"

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
	bool Frame();

private:
	bool Render(float a_Rotation);

private:
	Direct3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	
	TextureShaderClass* m_TextureShader;
	LightClass* m_LightClass;
};

#endif