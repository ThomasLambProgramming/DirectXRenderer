#include "applicationclass.h"

ApplicationClass::ApplicationClass()
{
}

ApplicationClass::ApplicationClass(ApplicationClass&)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initalize(int, int, HWND)
{
    return true;
}

void ApplicationClass::Shutdown()
{
    return;
}

bool ApplicationClass::Frame()
{
    return true;
}

bool ApplicationClass::Render()
{
    return true;
}
