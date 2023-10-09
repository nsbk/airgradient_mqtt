#ifndef Button_h
#define Button_h

#include "Arduino.h"

struct Button
{
    bool SingleClicked;
    bool LongPressed;

    // Get button feedback for a pin. Option to set a maximum time in milliseconds to get input
    static Button GetButtonInput(int pin, int timeout = 0)
    {
        Button result = Button();
        unsigned long pressedTime = 0;
        unsigned long releasedTime = 0;

        int shortpresstime = 2500;
        int longpresstime = 2500;

        int lastButtonState = LOW;
        bool isPressing = false;

        int loopStartTime = millis();
        while (!result.SingleClicked && !result.LongPressed)
        {
            // Pull-up resistor means a press will read as LOW - toggle to make it more intuitive
            int buttonState = !digitalRead(pin);

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
                    result.SingleClicked = true;
                }
            }

            if (isPressing)
            {
                long pressedDuration = millis() - pressedTime;
                if (pressedDuration > longpresstime)
                {
                    result.LongPressed = true;
                }
            }

            // If a max-wait is configured, break out if we've exceeded that length of time
            if ((timeout > 0) && (timeout <= (loopStartTime + millis())))
                break;

            delay(100);
            lastButtonState = buttonState;
        }

        return result;
    }
};

#endif