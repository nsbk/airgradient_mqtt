#ifndef ConfigModeSelector_h
#define ConfigModeSelector_h

#include "ConfigMode.h"

// Forward declaration to resolve circular dependency/include
class ConfigMode;

class ConfigModeSelector
{
public:
	ConfigModeSelector();
	// Same as before
	inline ConfigMode* getCurrentState() const { return currentState; }
	// In here, we'll delegate the state transition to the currentState
	void toggle();
	// This will get called by the current state
	void setState(ConfigMode& newState);

private:
	// ConfigMode here is now a class, not the enum that we saw earlier
	ConfigMode* currentState;
};

#endif