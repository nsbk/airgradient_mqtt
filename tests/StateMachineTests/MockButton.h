#ifndef MockButton_h
#define MockButton_h

#include "IButton.h"

namespace Input
{
    class MockButton : public IButton
    {
        public:
            // Get button feedback for the pin. Option to set a maximum time in milliseconds to get input
            void UpdateButtonInput(int timeout) override;

            // Reset button status
            void Reset() override;
    };
}

#endif