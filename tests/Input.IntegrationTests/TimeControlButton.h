#ifndef TimeControlButton_h
#define TimeControlButton_h

#include "Button.h"

namespace Input
{
    class TimeControlButton : public Button
    {
        protected:
            unsigned long getCurrentMillis() const override { return mMillis; }
        public:
            void setCurrentMillis(unsigned long ms) { mMillis = ms; }
            TimeControlButton(int timeout = 0) : Button(timeout) { };
        private:
            unsigned long mMillis;
    };
}

#endif