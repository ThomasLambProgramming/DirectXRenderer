#include "ApplicationDirector.h"

#include <iostream>

ApplicationDirector::ApplicationDirector()
{
	//always init everything to null.
	m_Input = nullptr;
	m_Application = nullptr;
}

ApplicationDirector::ApplicationDirector(const ApplicationDirector& other)
{
	//Some compilers can auto make copy constructors so we define one to avoid the possiblity.
}

//We dont do any clean up here, we do it in shutdown as singles exit thread functions and other things can actually not call destructors sometimes
//causing memory leaks. thats some new info for me.
ApplicationDirector::~ApplicationDirector()
{
}

bool ApplicationDirector::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	//Init the windows api
	InitializeWindows(screenWidth, screenHeight);
	
	//create and init the input object, this will take the keyboard input from the user.
	m_Input = new InputManager;
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
		return false;
	
	//Create and init the application class object. this will handle all the graphics for the application.
	m_Application = new ApplicationClass;
	
	//I assume the above is allowed without function () is because of not having any parameters in the constructor for both.

	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);

	
	
	return result;
}

void ApplicationDirector::Shutdown()
{
	//Release the application class obj
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = nullptr;
	}

	//release input obj
	if (m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	ShutdownWindows();
	return;
}

void ApplicationDirector::Run()
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
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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

LRESULT ApplicationDirector::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wparam, lparam))
        return true;
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

bool ApplicationDirector::Frame()
{
	//check if esc key pressed to exit application
	if (!m_Input->Frame())
		return false;

	if (!m_Application->Frame(m_Input))
		return false;
	
	return true;
}

void ApplicationDirector::InitializeWindows(int& screenWidth, int& screenHeight)
{
	//wndclassex is for the window class information
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//get an external pointer to this object
	ApplicationHandle = this;

	//Get the instance of this application
	m_hinstance = GetModuleHandle(nullptr);

	//set the application name the L is to set it to the wide string
	m_applicationName = L"Direct X Testing";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
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
		screenWidth = 1280;
		screenHeight = 720;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window with screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		m_applicationName,
		m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_CAPTION | WS_TABSTOP | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		posX,
		posY,
		screenWidth,
		screenHeight,
		nullptr,
		nullptr,
		m_hinstance,
	nullptr);

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


	RAWINPUTDEVICE rawInputDevices[1];
    rawInputDevices[0].usUsagePage = 0x01;  // Mouse
    rawInputDevices[0].usUsage = 0x02;      // Mouse
    rawInputDevices[0].dwFlags = 0;
    rawInputDevices[0].hwndTarget = m_hwnd;
    
    RegisterRawInputDevices(rawInputDevices, 1, sizeof(RAWINPUTDEVICE));
	
	return;
}

void ApplicationDirector::ShutdownWindows()
{
	//make sure the mouse cursor is shown
	ShowCursor(true);

	//if leaving full screen fix display settings
	if (FULL_SCREEN)
		ChangeDisplaySettings(nullptr, 0);

	//remove window
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	//remove the application instance
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;

	//release pointer to this class
	ApplicationHandle = nullptr;
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
		case WM_MOUSEMOVE:
		{
			XMFLOAT2 mousePosition = XMFLOAT2(LOWORD(lparam), HIWORD(lparam));
			if (ApplicationClass::Instance)
			{
				ApplicationClass::Instance->m_currentMousePos = mousePosition;
				//std::cout << "Mouse:" << ApplicationClass::Instance->m_currentMousePos.x << " : " << ApplicationClass::Instance->m_currentMousePos.y << std::endl;
			}
			break;
		}
		case WM_INPUT:
            UINT dataSize;
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER));

            if (dataSize > 0)
            {
                std::vector<BYTE> buffer(dataSize);
                if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, buffer.data(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
                {
                    RAWINPUT* rawInput = reinterpret_cast<RAWINPUT*>(buffer.data());

                    //if (rawInput->header.dwType == RIM_TYPEMOUSE && ApplicationClass::Instance != nullptr)
                    //{
	                //    ApplicationClass::Instance->m_currentMousePos.x = rawInput->data.mouse.lLastX;
                    //	ApplicationClass::Instance->m_currentMousePos.y = rawInput->data.mouse.lLastY;
                    //	std::cout << "Mouse:" << rawInput->data.mouse.lLastX << " : " << rawInput->data.mouse.lLastY << std::endl;
                    //}
                }
            }

		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
