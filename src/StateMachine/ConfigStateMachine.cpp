#include "ConfigStateMachine.h"

// Pin connected to AirGradient push button
#define BUTTON_PIN D7

using namespace Display;
using namespace Input;

ConfigStateMachine::ConfigStateMachine(IDisplay* display, IButton* button)
{
    this->display = display;
    this->button = button;
    // This is the default/starting state
    StateBase *newState = new SelectState();
    this->state = newState;
}

void ConfigStateMachine::Run()
{
    // 1. Take inputs

    // idea - could the length of this timeout be based around the current state? no timeout for some states...
    button->UpdateButtonInput(4000);

    // 2. Pass inputs to State
    if (button->LongPressed)
    {
        state->LongPress(*this);
        button->Reset();
    }
    else if (button->SingleClicked)
    {
        state->ShortPress(*this);
        button->Reset();
    }


    // 3. If a state transition occurs, take action based on the transition
}

// SelectState short-press rotates to the EditConfigState
void SelectState::ShortPress(MachineBase &machine)
{
    setState(machine, new EditConfigState());
}

// SelectState long-press has the same action as the short-press
void SelectState::LongPress(MachineBase &machine)
{
    setState(machine, new EditConfigState());
}

// SelectState short-press rotates to the ClearState
void EditConfigState::ShortPress(MachineBase &machine)
{
    setState(machine, new ClearState());
}

void EditConfigState::LongPress(MachineBase &machine)
{
    
}

// SelectState short-press rotates to the RebootState
void ClearState::ShortPress(MachineBase &machine)
{
    setState(machine, new RebootState());
}

void ClearState::LongPress(MachineBase &machine)
{
    
}

// SelectState short-press rotates back to the EditConfigState
void RebootState::ShortPress(MachineBase &machine)
{
    setState(machine, new EditConfigState());
}


void RebootState::LongPress(MachineBase &machine)
{
    // build a wrapper around this call
    //ESP.restart();
}