#ifndef ConfigStateMachine_h
#define ConfigStateMachine_h

#include "MachineBase.h"
#include "StateBase.h"
#include "../Display/Display.h"
#include "../Input/Input.h"
#include "../StringResources.h"

using namespace Display;
using namespace Input;

class ConfigStateMachine : public MachineBase
{
    public:
        ConfigStateMachine(IDisplay* display, IButton* button);
        void Run() override;


        // here or the base class - it should be possible to see the current state of the machine and to set it at the machine level, not the state level

    //     inline ConfigMode* getCurrentState() const { return currentState; }
	// // In here, we'll delegate the state transition to the currentState
	// void toggle();
	// // This will get called by the current state
	// void setState(ConfigMode& newState);

    private:
        IDisplay* display;
        IButton* button;

};

class SelectState : public StateBase
{
    // can this be a configstatemachine or does it have to be statemachine
    void ShortPress(MachineBase &machine) override;
    void LongPress(MachineBase &machine) override;
};

class EditConfigState : public StateBase
{
    void ShortPress(MachineBase &machine) override;
    void LongPress(MachineBase &machine) override;
};

class ClearState : public StateBase
{
    void ShortPress(MachineBase &machine) override;
    void LongPress(MachineBase &machine) override;
};

class RebootState : public StateBase
{
    void ShortPress(MachineBase &machine) override;
    void LongPress(MachineBase &machine) override;
};

#endif