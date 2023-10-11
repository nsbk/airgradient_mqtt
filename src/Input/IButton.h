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

            virtual void UpdateButtonInput(int timeout) = 0;
            virtual void Reset() = 0;
    };
}

#endif