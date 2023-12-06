#include "systemclass.h"

SystemClass::SystemClass()
{
	//always init everything to null.
	m_Input = 0;
	m_Application = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
	//Some compilers can auto make copy constructors so we define one to avoid the possiblity.
}

//We dont do any clean up here, we do it in shutdown as singles exit thread functions and other things can actually not call destructors sometimes
//causing memory leaks. thats some new info for me.
SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	//Init the windows api
	InitializeWindows(screenWidth, screenWidth);
	
	//create and init the input object, this will take the keyboard input from the user.
	m_Input = new InputClass;
	m_Input->Initialize();

	//Create and init the application class object. this will handle all the graphics for the application.
	m_Application = new ApplicationClass;
	
	//I assume the above is allowed without function () is because of not having any parameters in the constructor for both.

	result = m_Application->Initalize(screenWidth, screenHeight, m_hwnd);
	/*if (!result)
		return false;
	
	return false;*/
	return result;
}

void SystemClass::Shutdown()
{
	//Release the application class obj
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	//release input obj
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	ShutdownWindows();
	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//init message structure zero memory will fill the msg with 00000000 in memory but to the size specified. def:ZeroMemory fills a block of memory with zeros
	ZeroMemory(&msg, sizeof(MSG));

	//run the processing loop until a quit message from window or the user.
	done = false;
	while (!done)
	{
		//handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//if windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
			done = true;
		else
		{
			//otherwise we process the frame.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
	return;
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//check for key press on keyboard
		case WM_KEYDOWN:
		{
			//if a key is pressed down send it into the input object to set
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}
		case WM_KEYUP:
		{
			//if a key is pressed up send it into the input object to unset
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}
		//any other messages send to the default message handler as our application isnt going to use them.
		default:
			return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

bool SystemClass::Frame()
{
	bool result;

	//check if esc key pressed to exit application
	if (m_Input->IsKeyDown(VK_ESCAPE))
		return false;
	
	//do the frame processing for the application class obj
	result = m_Application->Frame();
	return result;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	//wndclassex is for the window class information
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//get an external pointer to this object
	ApplicationHandle = this;

	//Get the instance of this application
	m_hInstance = GetModuleHandle(NULL);

	//set the application name the L is to set it to the wide string
	m_applicationName = L"Direct X Testing";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//register the window class
	RegisterClassEx(&wc);

	//determine the resolution of the clients desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Setup the screen settings depending on whether it is running in full screen or windowed mode.
	if (FULL_SCREEN)
	{
		//set the screen to max size and 32 bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window with screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		m_applicationName,
		m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX,
		posY,
		screenWidth,
		screenHeight,
		NULL,
		NULL,
		m_hInstance,
		NULL);

	/*
	* // create the window and use the result as the handle
    hWnd = CreateWindowEx(NULL,
                          L"WindowClass1",    // name of the window class
                          L"Our First Windowed Program",   // title of the window
                          WS_OVERLAPPEDWINDOW,    // window style
                          300,    // x-position of the window
                          300,    // y-position of the window
                          500,    // width of the window
                          400,    // height of the window
                          NULL,    // we have no parent window, NULL
                          NULL,    // we aren't using menus, NULL
                          hInstance,    // application handle
                          NULL);    // used with multiple windows, NULL
	*/

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//Hide cursor
	ShowCursor(false);

	return;
	//What i get from this is that direct x has its own window manager/context unlike opengl where everyone uses glew and etc.
}

void SystemClass::ShutdownWindows()
{
	//make sure the mouse cursor is shown
	ShowCursor(true);

	//if leaving full screen fix display settings
	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	//remove window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//remove the application instance
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	//release pointer to this class
	ApplicationHandle = NULL;
	return;
}

//This is why we get the application handle of that specific object. so then the general message can be sent to the application while not keeping the message func
//in that specific object.
//We give this function to the window context object thingy so then windows can send the application messages.
LRESULT WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		case WM_DESTROY:
		case WM_CLOSE:
		{
			//check is the window is being destroyed
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
