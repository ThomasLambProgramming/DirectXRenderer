#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "InputClass.h"
#include "ApplicationClass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	//Return int, callback = __stdcall = function call setting on how parameters are to be done in memory.
	//this apparently gets sent windows system messages. kinda cool.
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	//basically a const wchar_t* Long Pointer to Constant Wide String lpc = long pointer constant, w = wide, str = string. wack
	LPCWSTR m_applicationName;

	//It specifies the application's Instance ID and is used in window creation. so im pre sure its just a fkn int, long long int as its 8 bytes
	//HInstance is a handle needed by window creation, menus, and a whole host of other functions to identify the correct program and instance when passing commands or requesting data. Each instance of a program gets it's own memory space, it's own set of handles, it's own stack and so on... this handle --not a pointer, a handle-- is how Windows knows what's where.
	HINSTANCE m_hinstance;

	//HWND is a window handle.
	//Technically its a intptr because of course it is.
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;
};

//Function prototypes
//params = handle to window, main message, additional message info, additional message into. returns result of message processing.
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Globals
static SystemClass* ApplicationHandle = 0;
#endif