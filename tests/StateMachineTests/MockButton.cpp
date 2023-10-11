#include "MockButton.h"

namespace Input
{
    // Update text on the display. Returns 0 once drawing is complete.
    void MockButton::UpdateButtonInput(int timeout = 0)
    {
        //todo
    }

    // Reset button status
    void MockButton::Reset()
    {
        this->SingleClicked = false;
        this->LongPressed = false;
    }
}