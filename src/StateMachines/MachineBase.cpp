#include "MachineBase.h"
#include "StateBase.h"

MachineBase::~MachineBase() { delete state; }