#ifndef ConfigStateMachine_h
#define ConfigStateMachine_h

#include "MachineBase.h"
#include "StateBase.h"
#include "../Display/IDisplay.h"

class ConfigStateMachine : MachineBase
{
    public:
        ConfigStateMachine(IDisplay* display);

    private:
        IDisplay* display;

};

class SelectState : StateBase
{
    // can this be a configstatemachine or does it have to be statemachine
    void ShortPress(MachineBase &machine) override;
    //void LongPress(ConfigStateMachine &machine) override;
};

// class EditConfigState : StateBase
// {
//     void ShortPress(ConfigStateMachine &machine) override;
//     void LongPress(ConfigStateMachine &machine) override;
// };

// class ClearState : StateBase
// {
//     void ShortPress(ConfigStateMachine &machine) override;
//     void LongPress(ConfigStateMachine &machine) override;
// };

// class RebootState : StateBase
// {
//     void ShortPress(ConfigStateMachine &machine) override;
//     void LongPress(ConfigStateMachine &machine) override;
// };

#endif