#include "MachineBase.h"
#include "StateBase.h"

MachineBase::MachineBase(const MachineType classType)
{
    SetClassType(classType);
}

void MachineBase::SetClassType(const MachineType classType)
{
    this->classType = classType;
}

MachineType MachineBase::GetClassType() const {return classType;}

void MachineBase::SetState(StateBase& newState)
{
    if (state != nullptr)
    {
        state->Exit(this);
    }
    state = &newState;
    state->Enter(this);
}

StateBase *MachineBase::GetState()
{
    return state;
}