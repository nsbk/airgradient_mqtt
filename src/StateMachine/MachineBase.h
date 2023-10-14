#ifndef MachineBase_h
#define MachineBase_h

class StateBase;

enum class MachineType
{
  BASE = 0,
  CONFIG = 1
};

class MachineBase
{
    public:
        virtual void Run() = 0;
        void SetState(StateBase& newState);
        StateBase *GetState();

        // todo - describe dynamic_cast-related issues
        MachineType GetClassType() const;
        void SetClassType(const MachineType classType);

        MachineBase(MachineType classType);

    protected:
        StateBase *state;

    private:
        MachineType classType;
};

#endif