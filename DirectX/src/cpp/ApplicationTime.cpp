#include "ApplicationTime.h"

ApplicationTime::ApplicationTime()
{
}

ApplicationTime::ApplicationTime(const ApplicationTime& a_Copy)
{
}

ApplicationTime::~ApplicationTime()
{
}

bool ApplicationTime::Initialize()
{
    INT64 frequency;

    //get cycles per second speed of this system.
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    if (frequency == 0)
    {
        return false;
    }
    m_frequency = (float)frequency;
    //get initial start time
    QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
    return true;
}

void ApplicationTime::Frame()
{
    INT64 currentTime;
    INT64 elapsedTicks;
    
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
    elapsedTicks = currentTime - m_startTime;
    m_deltaTime = (float)elapsedTicks / m_frequency;
    m_startTime = currentTime;
}

float ApplicationTime::GetDeltaTime()
{
    return m_deltaTime;
}
