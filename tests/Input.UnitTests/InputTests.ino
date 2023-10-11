#line 2 "InputTests.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <AUnitVerbose.h>
#include <Input.h>

using aunit::TestRunner;
using aunit::Verbosity;
using namespace Input;
// TODO namespace not implemented yet

#define BUTTON_PIN D7

test(ButtonTests, Constructor)
{
    IButton *testButton = new Button(BUTTON_PIN);

    assertNotEqual(nullptr, testButton);
}

test(ButtonTests, Reset)
{
    IButton *testButton = new Button(BUTTON_PIN);

    testButton->SingleClicked = true;
    testButton->LongPressed = true;
    testButton->Reset();

    assertFalse(testButton->SingleClicked);
    assertFalse(testButton->LongPressed);
}

void setup()
{
#if ! defined(EPOXY_DUINO)
    delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif
    SERIAL_PORT_MONITOR.begin(115200);
    while (!SERIAL_PORT_MONITOR); // needed for Leonardo/Micro

    TestRunner::setVerbosity(Verbosity::kAll);
}

void loop()
{
    TestRunner::run();
}
