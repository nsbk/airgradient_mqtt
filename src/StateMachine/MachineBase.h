#ifndef MachineBase_h
#define MachineBase_h

class StateBase;

class MachineBase
{
    // this is to enable States to acccess machine.state. is this really the best way to do it? maybe make it public in a safe way?
    friend class StateBase;
    
    public:
        virtual void Run() = 0;
        ~MachineBase();
        
    //protected:
        StateBase *state;
};

#endif