#include "InputClass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass&)
{
}

InputClass::~InputClass()
{
}
//HINSTANCE = handle to instance.
bool InputClass::Initialize(HINSTANCE hInstance, HWND windowHandle, int screenWidth, int screenHeight)
{
	HRESULT result;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_mouseX = 0;
	m_mouseY = 0;

	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
		return false;

	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
		return false;

	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	//Cooperative level is important. DISCL_EXCLUSIVE = no other program gets the input.
	//foreground = the device is unacquired automatically when the associated window moves to the background.
	result = m_keyboard->SetCooperativeLevel(windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	result = m_keyboard->Acquire();
	if (FAILED(result))
		return false;

	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
		return false;

	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
		return false;

	//Cooperative level is important. DISCL_EXCLUSIVE = no other program gets the input.
	//foreground = the device is unacquired automatically when the associated window moves to the background.
	result = m_mouse->SetCooperativeLevel(windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	result = m_mouse->Acquire();
	if (FAILED(result))
		return false;

	return true;
}

void InputClass::Shutdown()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

bool InputClass::Frame()
{
	bool result;
	result = ReadKeyboard();
	if (!result)
		return false;
	result = ReadMouse();
	if (!result)
		return false;

	ProcessInput();
	return true;
}

bool InputClass::IsEscapePressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is being pressed. (will later have to do a on down register and up register.
	return (m_keyboardState[DIK_ESCAPE] & 0x80);
}

void InputClass::GetMouseLocation(int& posX, int& posY)
{
	posX = m_mouseX;
	posY = m_mouseY;
}

bool InputClass::IsMousePressed(int mouseButton)
{
	switch(mouseButton)
	{
		case 0:
			return (m_mouseState.rgbButtons[0] & 0x80);
		//Dont really get where to get the actual mouse command / logic.
		case 1:
			return (m_mouseState.rgbButtons[0] & RI_MOUSE_RIGHT_BUTTON_DOWN);
		case 2:
			return (m_mouseState.rgbButtons[0] & RI_MOUSE_MIDDLE_BUTTON_DOWN);
		default:
			return false;
	}
}

bool InputClass::ReadKeyboard()
{
	//Keyboard can fail for one of 5 reaons, we only care about if focus is lost or it becomes unacquired, we call acquire each frame until control
	//has come back. if the window is minimized acquire will fail.
	HRESULT result;
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			m_keyboard->Acquire();
		else
			return false;
	}
	return true;
}

bool InputClass::ReadMouse()
{
	//the state of the mouse is just the delta of the mouse, this is somewhat annoying but at least we can track it to find out where it is.
	HRESULT result;

	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			m_mouse->Acquire();
		else
			return false;
	}
	return true;
}

void InputClass::ProcessInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	if (m_mouseX < 0)
		m_mouseX = 0;
	if (m_mouseY < 0)
		m_mouseY = 0;

	if (m_mouseX > m_screenWidth)
		m_mouseX = m_screenWidth;
	if (m_mouseY > m_screenHeight)
		m_mouseY = m_screenHeight;
}