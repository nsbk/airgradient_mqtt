#line 2 "StateMachineTests.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <StateMachine.h>
#include "MockDisplay.h"

using aunit::TestRunner;
// namespace not implemented yet
//using namespace StateMachine;

test(U8g2DisplayTests, Constructor)
{
    IDisplay *mockDisplay = new MockDisplay();
    MachineBase *testMachine = new ConfigStateMachine(mockDisplay);

    assertNotEqual(nullptr, testMachine);
}

void setup()
{
#if ! defined(EPOXY_DUINO)
    delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif
    SERIAL_PORT_MONITOR.begin(115200);
    while (!SERIAL_PORT_MONITOR); // needed for Leonardo/Micro
}

void loop()
{
    TestRunner::run();
}
