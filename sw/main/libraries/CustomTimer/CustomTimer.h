#ifndef CUSTOMTIMER_H_
#define CUSTOMTIMER_H_

#include "Arduino.h"

class CustomTimer
{
    public:
        CustomTimer();
        CustomTimer(unsigned long waitTime);
        void Init(unsigned long waitTime);
        bool Finished();
        unsigned long ReadElapsed();
        unsigned long ReadRemaining();
    
    private:
        unsigned long InitTime;
        unsigned long WaitTime;
        bool Counting;

};

#endif