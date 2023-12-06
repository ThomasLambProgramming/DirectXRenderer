#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include <Windows.h>

//GLOBALS
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(ApplicationClass&);
	~ApplicationClass();

	bool Initalize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
};

#endif