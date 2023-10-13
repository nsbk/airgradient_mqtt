#include "ConfigStateMachine.h"
#include "Arduino.h"

// Pin connected to AirGradient push button
#define BUTTON_PIN D7

using namespace Display;
using namespace Input;

ConfigStateMachine::ConfigStateMachine(IDisplay* display, IButton* button)
{
    Serial.println("configstatemachine constructor");
    this->display = display;
    this->button = button;
    // This is the default/starting state
    this->state = new SelectState();
    // StateBase *newState = new SelectState();
    // this->SetState(newState);

    //StateBase newState = SelectState();
    //this->SetState(*newState);

    this->WriteToDisplay("test", "test", "test");
}

void ConfigStateMachine::Run()
{
    // 1. Take inputs

    // idea - could the length of this timeout be based around the current state? no timeout for some states...
    button->UpdateButtonInput(4000);

    // 2. Pass inputs to State
    if (button->LongPressed)
    {
        //state->LongPress(*this);
        state->LongPress(this);
        button->Reset();
    }
    else if (button->SingleClicked)
    {
        //state->ShortPress(*this);
        state->ShortPress(this);
        button->Reset();
    }


    // 3. If a state transition occurs, take action based on the transition
}

int ConfigStateMachine::WriteToDisplay(String line1, String line2, String line3)
{
    return display->WriteLines(line1, line2, line3);
}

// SelectState short-press rotates to the EditConfigState
//void SelectState::ShortPress(MachineBase &machine)
void SelectState::ShortPress(MachineBase* machine)
{
    //setState(machine, new EditConfigState());
    machine->SetState(EditConfigState::GetInstance());
}

// SelectState long-press has the same action as the short-press
//void SelectState::LongPress(MachineBase &machine)
void SelectState::LongPress(MachineBase* machine)
{
    //setState(machine, new EditConfigState());
    machine->SetState(EditConfigState::GetInstance());
}

// SelectState short-press rotates to the ClearState
//void EditConfigState::ShortPress(MachineBase &machine)
void EditConfigState::ShortPress(MachineBase* machine)
{
    //setState(machine, new ClearState());
    machine->SetState(ClearState::GetInstance());
}

//void EditConfigState::LongPress(MachineBase &machine)
void EditConfigState::LongPress(MachineBase* machine)
{
    
}

// SelectState short-press rotates to the RebootState
//void ClearState::ShortPress(MachineBase &machine)
void ClearState::ShortPress(MachineBase* machine)
{
    //setState(machine, new RebootState());
    machine->SetState(RebootState::GetInstance());
}

//void ClearState::LongPress(MachineBase &machine)
void ClearState::LongPress(MachineBase* machine)
{
    
}

// SelectState short-press rotates back to the EditConfigState
//void RebootState::ShortPress(MachineBase &machine)
void RebootState::ShortPress(MachineBase* machine)
{
    //setState(machine, new EditConfigState());
    machine->SetState(EditConfigState::GetInstance());
}

//void RebootState::LongPress(MachineBase &machine)
void RebootState::LongPress(MachineBase* machine)
{
    // build a wrapper around this call
    //ESP.restart();
}


StateBase& SelectState::GetInstance()
{
    static SelectState singleton;
    return singleton;
}

StateBase& EditConfigState::GetInstance()
{
    static EditConfigState singleton;
    return singleton;
}

StateBase& ClearState::GetInstance()
{
    static ClearState singleton;
    return singleton;
}

StateBase& RebootState::GetInstance()
{
    static RebootState singleton;
    return singleton;
}

// void SelectState::Enter(ConfigStateMachine* machine) 
// {
//     // Config-specific Enter logic
//     machine->WriteToDisplay(
//         OLEDStrings::SelectStateLine1,
//         OLEDStrings::SelectStateLine2,
//         OLEDStrings::SelectStateLine3
//     );

//     // Finish-out with any base-class logic.
//     MachineBase* machineBase = machine;
//     this->Enter(machineBase);
// }
void SelectState::Enter(MachineBase* machine) 
{
    // Finish-out with any base-class logic.
    ConfigStateMachine* configMachine = dynamic_cast<ConfigStateMachine*>(machine);
    
    configMachine->WriteToDisplay(
        OLEDStrings::SelectStateLine1,
        OLEDStrings::SelectStateLine2,
        OLEDStrings::SelectStateLine3
    );
}


void EditConfigState::Enter(MachineBase* machine) {}
void ClearState::Enter(MachineBase* machine) {}
void RebootState::Enter(MachineBase* machine) {}
void SelectState::Exit(MachineBase* machine) {}
void EditConfigState::Exit(MachineBase* machine) {}
void ClearState::Exit(MachineBase* machine) {}
void RebootState::Exit(MachineBase* machine) {}