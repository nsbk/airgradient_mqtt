#ifndef TypeConversionFunctions_h
#define TypeConversionFunctions_h

#include "MachineBase.h"
#include "ConfigStateMachine.h"

namespace StateMachineTypeConversionFunctions 
{
    /**
     * Conversion function that can be used on Machine classes instead of dynamic_cast since RTTI is disabled.
     * 
     * @param T The Machine class pointer type to cast the machineBaseInstance pointer into.
     * @param machineBaseInstance The instance pointer to cast.
     * @return A pointer of type T to machineBaseInstance if machineBaseInstance is of type T. nullptr otherwise.
     */
    template <typename T>
  
    T machineCast(MachineBase *machineBaseInstance)
    {
        // The only valid template arguments are handled by the template specializations below, so ending up here is an error.
        static_assert(std::is_same<T, ConfigStateMachine *>::value,
                  "Error: Invalid Machine class type. Make sure the template argument to machineCast is supported!");
    }

    template <> 
    ConfigStateMachine *machineCast<ConfigStateMachine *>(MachineBase *machineBaseInstance);
  
}

#endif