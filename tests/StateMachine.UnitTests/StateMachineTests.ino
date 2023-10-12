#line 2 "StateMachineTests.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <AUnitVerbose.h>
#include <StateMachine.h>
#include "MockDisplay.h"
#include "MockButton.h"

using aunit::TestRunner;
using aunit::Verbosity;
// TODO namespace not implemented yet
//using namespace StateMachine;

test(StateMachineTests, Constructor_and_Main_State_Transitions)
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

    // todo - for future readability, break these out into separate tests that leverage a test fixture and each configure the previous, expected state
    //https://github.com/bxparks/AUnit/tree/develop#test-fixtures
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

    bool loopedBackToEditState = false;
    testMachine->state->ShortPress(*testMachine);
    if (dynamic_cast<EditConfigState*>(testMachine->state) != nullptr)
    {
        loopedBackToEditState = true;
    }
    assertTrue(loopedBackToEditState);
}

test(StateMachineTests, RunMachine)
{
    // Arrange
    IDisplay *mockDisplay = new MockDisplay();
    IButton *mockButton = new MockButton();
    MachineBase *testMachine = new ConfigStateMachine(mockDisplay, mockButton);

    // ConfigStateMachines should begin with the SelectState.
    bool correctInitialState = false;
    if (dynamic_cast<SelectState*>(testMachine->state) != nullptr)
    {
        correctInitialState = true;
    }
    assertTrue(correctInitialState);

    mockButton->SingleClicked = true;

    // Act
    testMachine->Run();

    // Assert

    // todo - for future readability, break these out into separate tests that leverage a test fixture and each configure the previous, expected state
    //https://github.com/bxparks/AUnit/tree/develop#test-fixtures
    bool enteredEditConfigState = false;
    if (dynamic_cast<EditConfigState*>(testMachine->state) != nullptr)
    {
        enteredEditConfigState = true;
    }
    assertTrue(enteredEditConfigState);

    // The final step of machine.run is to reset the button state.
    assertFalse(mockButton->SingleClicked);
    assertFalse(mockButton->LongPressed);
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
