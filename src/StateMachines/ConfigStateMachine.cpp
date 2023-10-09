#include "ConfigStateMachine.h"
#include "../Button.h"

// Pin connected to AirGradient push button
#define BUTTON_PIN D7

using namespace Display;

ConfigStateMachine::ConfigStateMachine(IDisplay* display)
{
    this->display = display;
    // This is the default/starting state
    StateBase *newState = new SelectState();
    this->state = newState;
}

void ConfigStateMachine::Run()
{
    // 1. Take inputs

    // idea - could the length of this timeout be based around the current state? no timeout for some states...
    Button pushButton = Button::GetButtonInput(BUTTON_PIN, 4000);

    // 2. Pass inputs to State
    if (pushButton.LongPressed)
    {
        state->LongPress(*this);
    }
    else if (pushButton.SingleClicked)
    {
        state->ShortPress(*this);
    }


    // 3. If a state transition occurs, take action based on the transition
}

void SelectState::ShortPress(MachineBase &machine)
{
    setState(machine, new EditConfigState());
}

void SelectState::LongPress(MachineBase &machine)
{
    
}

void EditConfigState::ShortPress(MachineBase &machine)
{
    setState(machine, new ClearState());
}

void EditConfigState::LongPress(MachineBase &machine)
{
    
}

void ClearState::ShortPress(MachineBase &machine)
{
    setState(machine, new RebootState());
}

void ClearState::LongPress(MachineBase &machine)
{
    
}

void RebootState::ShortPress(MachineBase &machine)
{
    setState(machine, new SelectState());
}

void RebootState::LongPress(MachineBase &machine)
{
    
}