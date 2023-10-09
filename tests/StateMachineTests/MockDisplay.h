#ifndef MockDisplay_h
#define MockDisplay_h

#include <WString.h>
#include "IDisplay.h"

namespace Display
{
    class MockDisplay : public IDisplay
    {
        public:
            // Update text on the display
            int WriteLines(String line1, String line2, String line3) override;
    };
}

#endif