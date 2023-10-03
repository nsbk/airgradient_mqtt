#ifndef U8g2Display_h
#define U8g2Display_h

#include <U8g2lib.h>
#include <WString.h>
#include "IDisplay.h"

class U8g2Display : public IDisplay
{
    private: 
        U8G2 u8g2;

    public:
        void WriteLines(String line1, String line2, String line3) override;

        U8g2Display(U8G2 display);
};

#endif