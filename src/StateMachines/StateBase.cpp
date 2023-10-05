#include "StateBase.h"
#include "MachineBase.h"

void StateBase::setState(MachineBase &machine, StateBase *state)
{
    StateBase *aux = machine.state;
    machine.state = state;
    delete aux;
}