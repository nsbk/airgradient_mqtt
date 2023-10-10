#line 2 "StateMachineTests.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <StateMachine.h>
#include "MockDisplay.h"

using aunit::TestRunner;
// TODO namespace not implemented yet
//using namespace StateMachine;

test(StateMachineTests, Constructor_and_Main_State_Transitions)
{
    IDisplay *mockDisplay = new MockDisplay();
    MachineBase *testMachine = new ConfigStateMachine(mockDisplay);

    assertNotEqual(nullptr, testMachine);

    // ConfigStateMachines should begin with the SelectState.
    bool correctInitialState = false;
    if (dynamic_cast<SelectState*>(testMachine->state) != nullptr)
    {
        correctInitialState = true;
    }
    assertTrue(correctInitialState);

    // todo - for future readability, break these out into separate tests that leverage a test fixture and each configure the previous, expected state
    bool enteredEditConfigState = false;
    testMachine->state->ShortPress(*testMachine);
    if (dynamic_cast<EditConfigState*>(testMachine->state) != nullptr)
    {
        enteredEditConfigState = true;
    }
    assertTrue(enteredEditConfigState);

    bool enteredClearState = false;
    testMachine->state->ShortPress(*testMachine);
    if (dynamic_cast<ClearState*>(testMachine->state) != nullptr)
    {
        enteredClearState = true;
    }
    assertTrue(enteredClearState);

    bool enteredRebootState = false;
    testMachine->state->ShortPress(*testMachine);
    if (dynamic_cast<RebootState*>(testMachine->state) != nullptr)
    {
        enteredRebootState = true;
    }
    assertTrue(enteredRebootState);

    bool loopedBackToInitialState = false;
    testMachine->state->ShortPress(*testMachine);
    if (dynamic_cast<SelectState*>(testMachine->state) != nullptr)
    {
        loopedBackToInitialState = true;
    }
    assertTrue(loopedBackToInitialState);
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
