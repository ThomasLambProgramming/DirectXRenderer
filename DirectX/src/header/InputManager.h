#pragma once

//Specify that it is version 8 or there will be alot of annoying warning messages
#define DIRECTINPUT_VERSION 0x0800

//tells linker to add libname library to the list of library dependencies.
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <dinput.h>

class InputManager
{
public:
	InputManager();
	InputManager(const InputManager&);
	~InputManager();

	bool Initialize(HINSTANCE hInstance, HWND windowHandle, int screenWidth, int screenHeight);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int& posX, int& posY);
	//0 = left, 1 = right, 2 = middle
	bool IsMousePressed(int mouseButton);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth;
	int m_screenHeight;
	int m_mouseX;
	int m_mouseY;
};