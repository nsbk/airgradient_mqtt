#ifndef LocalStorageConfigManager_h
#define LocalStorageConfigManager_h

// Leverage EpoxyDuino to mock the onboard flash storage
#if defined(EPOXY_DUINO)
#include <EpoxyEepromEsp.h>
#include <EpoxyFS.h>
#else
#include <EEPROM.h>
#include <LittleFS.h>
#endif

#include <WString.h>
#include "../IConfigManager.h"

namespace Data::Local
{
    class LocalStorageConfigManager : public IConfigManager
    {
        private: 
            int addr = 4;
            String filename = "/config.json";

        public:
            int ReadDisplayConfigurationMode() override;
            bool WriteDisplayConfigurationMode(int mode) override;
            std::map<String, String> ReadServerConfiguration() override;
            int WriteServerConfiguration(std::map<String, String> configuration) override;

            // todo
            // take a key (like "mqtt_server") and a value (char array) to read and write data
            // implement that in this class using json
            // rename this from ConfigManager to just local storage
            // wrap this class in another that handles Configuration stuff
            // wrap it in the config manager

            LocalStorageConfigManager();
    };
}

#endif