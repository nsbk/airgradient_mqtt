#include "ConfigStateMachine.h"
#include "TypeConversionFunctions.h"

// Pin connected to AirGradient push button
#define BUTTON_PIN D7

using namespace Display;
using namespace Input;
namespace TypeCast = StateMachineTypeConversionFunctions;

ConfigStateMachine::ConfigStateMachine(IDisplay* display, IButton* button) : MachineBase(MachineType::CONFIG)
{
    this->display = display;
    this->button = button;
    
    // This is the default/starting state
    StateBase *newState = new SelectState();
    this->SetState(*newState);
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

#pragma region State_GetInstance_Functions

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

#pragma endregion State_GetInstance_Functions

#pragma region State_Enter_Functions

void SelectState::Enter(MachineBase* machine) 
{
    // ConfigStateMachine* configMachine = dynamic_cast<ConfigStateMachine*>(machine);
    if (ConfigStateMachine *configStateMachineInstance = TypeCast::machineCast<ConfigStateMachine *>(machine))
    {
        configStateMachineInstance->WriteToDisplay(
            OLEDStrings::SelectStateLine1,
            OLEDStrings::SelectStateLine2,
            OLEDStrings::SelectStateLine3
        );
    }
}


void EditConfigState::Enter(MachineBase* machine) 
{
    if (ConfigStateMachine *configStateMachineInstance = TypeCast::machineCast<ConfigStateMachine *>(machine))
    {
        configStateMachineInstance->WriteToDisplay(
            OLEDStrings::EditConfigStateLine1,
            OLEDStrings::EditConfigStateLine2,
            OLEDStrings::EditConfigStateLine3
        );
    }
}

void ClearState::Enter(MachineBase* machine) 
{
    if (ConfigStateMachine *configStateMachineInstance = TypeCast::machineCast<ConfigStateMachine *>(machine))
    {
        configStateMachineInstance->WriteToDisplay(
            OLEDStrings::ClearConfigStateLine1,
            OLEDStrings::ClearConfigStateLine2,
            OLEDStrings::ClearConfigStateLine3
        );
    }
}
void RebootState::Enter(MachineBase* machine) 
{
    if (ConfigStateMachine *configStateMachineInstance = TypeCast::machineCast<ConfigStateMachine *>(machine))
    {
        configStateMachineInstance->WriteToDisplay(
            OLEDStrings::RebootStateLine1,
            OLEDStrings::RebootStateLine2,
            OLEDStrings::RebootStateLine3
        );
    }
}
#pragma endregion State_Enter_Functions

#pragma region State_Exit_Functions

void SelectState::Exit(MachineBase* machine) {}
void EditConfigState::Exit(MachineBase* machine) {}
void ClearState::Exit(MachineBase* machine) {}
void RebootState::Exit(MachineBase* machine) {}

#pragma endregion State_Exit_Functions