#ifndef IDisplay_h
#define IDisplay_h

#include <WString.h>

class IDisplay
{
    public:
        virtual void WriteLines(String line1, String line2, String line3) = 0;
};

#endif