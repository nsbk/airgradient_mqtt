#ifndef MachineBase_h
#define MachineBase_h

class StateBase;

class MachineBase
{
    public:
        virtual void Run() = 0;
        void SetState(StateBase& newState);
        StateBase *GetState();

    protected:
        StateBase *state;
};

#endif