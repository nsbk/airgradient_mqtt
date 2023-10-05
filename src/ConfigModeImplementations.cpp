#include "ConfigMode.h"
#include "ConfigModeImplementations.h"

void SelectionMode::toggle(ConfigModeSelector* configModeSelector)
{
	// Off -> Low
	configModeSelector->setState(EditDisplay::getInstance());
}

ConfigMode& SelectionMode::getInstance()
{
	static SelectionMode singleton;
	return singleton;
}

void EditDisplay::toggle(ConfigModeSelector* configModeSelector)
{
	// Low -> Medium
	configModeSelector->setState(ClearData::getInstance());
}

ConfigMode& EditDisplay::getInstance()
{
	static EditDisplay singleton;
	return singleton;
}

void ClearData::toggle(ConfigModeSelector* configModeSelector)
{
	// Medium -> High
	configModeSelector->setState(RebootDevice::getInstance());
}

ConfigMode& ClearData::getInstance()
{
	static ClearData singleton;
	return singleton;
}

void RebootDevice::toggle(ConfigModeSelector* configModeSelector)
{
	// High -> Low
	configModeSelector->setState(SelectionMode::getInstance());
}

ConfigMode& RebootDevice::getInstance()
{
	static RebootDevice singleton;
	return singleton;
}