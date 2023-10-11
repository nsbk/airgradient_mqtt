#ifndef IButton_h
#define IButton_h

namespace Input
{
    // This interface provides input from one button.
    class IButton
    {
        public:
            bool SingleClicked;
            bool LongPressed;

            //void GetButtonInput(int pin, int timeout = 0) = 0;
    };
}

#endif