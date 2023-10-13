#include "LocalStorageConfigManager.h"

// Leverage EpoxyDuino to mock the onboard flash storage
#if defined(EPOXY_DUINO)
#define EEPROM EpoxyEepromEspInstance
#endif

namespace Data::Local
{
    int LocalStorageConfigManager::ReadDisplayConfigurationMode()
    {
        return String(EEPROM.read(addr)).toInt();
    };
    int LocalStorageConfigManager::WriteDisplayConfigurationMode(int mode)
    {
        EEPROM.write(addr, char(mode));
        EEPROM.commit();
    };

    LocalStorageConfigManager::LocalStorageConfigManager()
    {
        EEPROM.begin(512);
    }
}