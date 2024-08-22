#include "CustomTimer.h"

CustomTimer::CustomTimer()
{
	InitTime = 0;
    WaitTime = 0;
    Counting = false;
}

CustomTimer::CustomTimer(unsigned long waitTime)
{
	InitTime = millis();
    WaitTime = waitTime;
    Counting = true;
}

void CustomTimer::Init(unsigned long waitTime)
{      
    InitTime = millis();
    WaitTime = waitTime;
    Counting = true;
}

unsigned long CustomTimer::ReadElapsed()
{
    unsigned long NowTime =  millis();
    return NowTime - InitTime;
}

unsigned long CustomTimer::ReadRemaining()
{
    unsigned long NowTime =  millis();
    return WaitTime - ReadElapsed();
}

bool CustomTimer::Finished()
{
    if(ReadElapsed() >= WaitTime || !Counting)
    {
        Counting = false;
        return true;
    }
    else
    {
        return false;
    }
}