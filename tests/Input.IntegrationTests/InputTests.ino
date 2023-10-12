#line 2 "InputTests.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <AUnitVerbose.h>
#include <Input.h>
#include "TimeControlButton.h"

using aunit::TestRunner;
using aunit::Verbosity;
using namespace Input;
// TODO namespace not implemented yet

#define BUTTON_PIN D7

test(ButtonTests, Constructor)
{
    IButton *testButton = new TimeControlButton(BUTTON_PIN);

    assertNotEqual(nullptr, testButton);
}

test(ButtonTests, ShortPress)
{
    TimeControlButton *testButton = new TimeControlButton(BUTTON_PIN);

    testButton->setCurrentMillis = 0;
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