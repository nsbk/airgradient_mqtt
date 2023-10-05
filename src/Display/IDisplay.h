#ifndef IDisplay_h
#define IDisplay_h

#include <WString.h>

// This interface provides communication with external displays.
class IDisplay
{
    public:
        // Update text on the display
        virtual void WriteLines(String line1, String line2, String line3) = 0;
};

#endif