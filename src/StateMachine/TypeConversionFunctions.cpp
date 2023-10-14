#include "TypeConversionFunctions.h"

namespace StateMachineTypeConversionFunctions 
{
    /**
    * Helper function for machineCast.
    */
    template <typename T>
    T attemptPointerCast(MachineBase *machineBaseInstance, MachineType resultClassType)
    {
        if(machineBaseInstance && machineBaseInstance->GetClassType() == resultClassType)
        {
        return static_cast<T>(machineBaseInstance); 
        }
        
        return nullptr;
    }
    
    template <> 
    ConfigStateMachine *machineCast<ConfigStateMachine *>(MachineBase *machineBaseInstance)
    {
        return attemptPointerCast<ConfigStateMachine *>(machineBaseInstance, MachineType::CONFIG);
    }
}