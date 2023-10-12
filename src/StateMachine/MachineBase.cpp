#include "MachineBase.h"
#include "StateBase.h"

void MachineBase::SetState(StateBase& newState)
{
    state->Exit(this);
    state = &newState;
    state->Enter(this);
}

StateBase *MachineBase::GetState()
{
    return state;
}