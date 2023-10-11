#line 2 "StateMachineTests.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <StateMachine.h>
#include "MockDisplay.h"
#include "MockButton.h"

using aunit::TestRunner;
// TODO namespace not implemented yet
//using namespace StateMachine;

test(StateMachineTests, Constructor)
{
    IDisplay *mockDisplay = new MockDisplay();
    IButton *mockButton = new MockButton();
    MachineBase *testMachine = new ConfigStateMachine(mockDisplay, mockButton);

    assertNotEqual(nullptr, testMachine);

    // ConfigStateMachines should begin with the SelectState.
    bool correctInitialState = false;
    if (dynamic_cast<SelectState*>(testMachine->state) != nullptr)
    {
        correctInitialState = true;
    }
    assertTrue(correctInitialState);
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
