#ifndef StateBase_h
#define StateBase_h

class MachineBase;

class StateBase
{
    public:
        virtual void ShortPress(MachineBase &machine) = 0;
        //virtual void LongPress(MachineBase &machine) = 0;

    protected:
        void setState(MachineBase &machine, StateBase *state);
};

#endif