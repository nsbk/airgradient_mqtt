#ifndef ConfigModeImplementations_h
#define ConfigModeImplementations_h

#include "ConfigMode.h"
#include "ConfigModeSelector.h"

class SelectionMode : public ConfigMode
{
public:
	void enter(ConfigModeSelector* configModeSelector) {}
	void toggle(ConfigModeSelector* configModeSelector);
	void exit(ConfigModeSelector* configModeSelector) {}
	static ConfigMode& getInstance();

private:
	SelectionMode() {}
	SelectionMode(const SelectionMode& other);
	SelectionMode& operator=(const SelectionMode& other);
};

class EditDisplay : public ConfigMode
{
public:
	void enter(ConfigModeSelector* configModeSelector) {}
	void toggle(ConfigModeSelector* configModeSelector);
	void exit(ConfigModeSelector* configModeSelector) {}
	static ConfigMode& getInstance();

private:
	EditDisplay() {}
	EditDisplay(const EditDisplay& other);
	EditDisplay& operator=(const EditDisplay& other);
};

class ClearData : public ConfigMode
{
public:
	void enter(ConfigModeSelector* configModeSelector) {}
	void toggle(ConfigModeSelector* configModeSelector);
	void exit(ConfigModeSelector* configModeSelector) {}
	static ConfigMode& getInstance();

private:
	ClearData() {}
	ClearData(const ClearData& other);
	ClearData& operator=(const ClearData& other);
};

class RebootDevice : public ConfigMode
{
public:
	void enter(ConfigModeSelector* configModeSelector) {}
	void toggle(ConfigModeSelector* configModeSelector);
	void exit(ConfigModeSelector* configModeSelector) {}
	static ConfigMode& getInstance();

private:
	RebootDevice() {}
	RebootDevice(const RebootDevice& other);
	RebootDevice& operator=(const RebootDevice& other);
};

#endif