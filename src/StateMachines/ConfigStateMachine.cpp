#include "ConfigStateMachine.h"

ConfigStateMachine::ConfigStateMachine(IDisplay* display)
{
    this->display = display;
    // This is the default/starting state
    //StateBase *newState = new SelectState();
    //this->state = newState;
}

void SelectState::ShortPress(MachineBase &machine)
{
    //setState(machine, new EditConfigState());
}

// void SelectState::LongPress(MachineBase &machine)
// {
    
// }

// void EditConfigState::ShortPress(MachineBase &machine)
// {
//     setState(machine, new ClearState());
// }

// void EditConfigState::LongPress(MachineBase &machine)
// {
    
// }

// void ClearState::ShortPress(MachineBase &machine)
// {
//     setState(machine, new RebootState());
// }

// void ClearState::LongPress(MachineBase &machine)
// {
    
// }

// void RebootState::ShortPress(MachineBase &machine)
// {
//     setState(machine, new SelectState());
// }

// void RebootState::LongPress(MachineBase &machine)
// {
    
// }