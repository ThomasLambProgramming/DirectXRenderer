#pragma once

#include <Windows.h>

class ApplicationTime
{
public:
    ApplicationTime();
    ApplicationTime(const ApplicationTime& a_Copy);
    ~ApplicationTime();

    bool Initialize();
    void Frame();

    float GetDeltaTime();
private:
    float m_frequency;
    INT64 m_startTime;
    float m_deltaTime;
};
