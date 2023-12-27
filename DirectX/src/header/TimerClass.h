#pragma once

#include <Windows.h>

class TimerClass
{
public:
    TimerClass();
    TimerClass(const TimerClass& a_Copy);
    ~TimerClass();

    bool Initialize();
    void Frame();

    float GetDeltaTime();
private:
    float m_frequency;
    INT64 m_startTime;
    float m_deltaTime;
};
