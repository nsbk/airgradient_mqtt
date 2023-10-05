#include "ConfigModeSelector.h"
#include "ConfigModeImplementations.h"

ConfigModeSelector::ConfigModeSelector()
{
  // default state
	currentState = &SelectionMode::getInstance();
}

void ConfigModeSelector::setState(ConfigMode& newState)
{
	currentState->exit(this);  // do something before we change state
	currentState = &newState;  // actually change states now
	currentState->enter(this); // do something after we change state
}

void ConfigModeSelector::toggle()
{
	// Delegate the task of determining the next state to the current state!
	currentState->toggle(this);
}