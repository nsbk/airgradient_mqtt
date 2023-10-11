#include "Arduino.h"
#include "Button.h"

namespace Input
{
    Button::Button(int pin)
    {
        this->buttonPin = pin;
        pinMode(pin, INPUT_PULLUP);
    }

    // Get button feedback for the pin. Option to set a maximum time in milliseconds to get input
    void Button::UpdateButtonInput(int timeout = 0)
    {
        unsigned long pressedTime = 0;
        unsigned long releasedTime = 0;

        int shortpresstime = 2500;
        int longpresstime = 2500;

        int lastButtonState = LOW;
        bool isPressing = false;

        int loopStartTime = millis();
        while (!this->SingleClicked && !this->LongPressed)
        {
            // Pull-up resistor means a press will read as LOW - toggle to make it more intuitive
            int buttonState = !digitalRead(buttonPin);

            // Button is pressed
            if (lastButtonState == LOW && buttonState == HIGH)
            {
                pressedTime = millis();
                isPressing = true;
            }

            // Button is released - could be a single click
            else if (lastButtonState == HIGH && buttonState == LOW)
            {
                isPressing = false;
                releasedTime = millis();
                long pressDuration = releasedTime - pressedTime;
                if (pressDuration < shortpresstime)
                {
                    this->SingleClicked = true;
                }
            }

            if (isPressing)
            {
                long pressedDuration = millis() - pressedTime;
                if (pressedDuration > longpresstime)
                {
                    this->LongPressed = true;
                }
            }

            // If a max-wait is configured, break out if we've exceeded that length of time
            if ((timeout > 0) && (timeout <= (loopStartTime + millis())))
                break;

            delay(100);
            lastButtonState = buttonState;
        }
    }
}



