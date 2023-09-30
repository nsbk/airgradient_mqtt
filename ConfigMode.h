#ifndef ConfigMode_h
#define ConfigMode_h

#include "ConfigModeSelector.h"

// Forward declaration to resolve circular dependency/include
class ConfigModeSelector;

class ConfigMode
{
public:
	virtual void enter(ConfigModeSelector* configModeSelector) = 0;
	virtual void toggle(ConfigModeSelector* configModeSelector) = 0;
	virtual void exit(ConfigModeSelector* configModeSelector) = 0;
	virtual ~ConfigMode() {}
};

#endif