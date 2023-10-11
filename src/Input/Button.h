#ifndef Button_h
#define Button_h

#include "Arduino.h"
#include "IButton.h"

namespace Input
{
    class Button : public IButton
    {
        private:
            int buttonPin = 0;

        public:
            Button(int pin);

            // Get button feedback for the pin. Option to set a maximum time in milliseconds to get input
            void UpdateButtonInput(int timeout);
    };
}
#endif