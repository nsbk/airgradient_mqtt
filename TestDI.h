#ifndef TestDI_h
#define TestDI_h

#include <WString.h>
#include <Arduino.h>
#include "IDisplay.h"

class TestDI
{
    public:
        void DoTheTest();
        TestDI(IDisplay* display);

    private:
        IDisplay* display;
};

#endif