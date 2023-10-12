#ifndef StateBase_h
#define StateBase_h

class MachineBase;

class StateBase
{
    public:
        virtual void Enter(MachineBase* machine) = 0;
        
        virtual void ShortPress(MachineBase* machine) = 0;
        virtual void LongPress(MachineBase* machine) = 0;
        
        virtual void Exit(MachineBase* machine) = 0;
};

#endif