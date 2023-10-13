#include "MockDisplay.h"

namespace Display
{
    // Update text on the display. Returns 0 once drawing is complete.
    int MockDisplay::WriteLines(String line1, String line2, String line3)
    {
        this->Line1 = line1;
        this->Line2 = line2;
        this->Line3 = line3;
        return 0;
    }
}