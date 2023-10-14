#include "LocalStorageConfigManager.h"
#include <ArduinoJson.h>

// Leverage EpoxyDuino to mock the onboard flash storage
#if defined(EPOXY_DUINO)
#define EEPROM EpoxyEepromEspInstance
#define FILE_SYSTEM fs::EpoxyFS
#else
#define FILE_SYSTEM LittleFS
#endif



namespace Data::Local
{
    int LocalStorageConfigManager::ReadDisplayConfigurationMode()
    {
        return String(EEPROM.read(addr)).toInt();
    }

    bool LocalStorageConfigManager::WriteDisplayConfigurationMode(int mode)
    {
        EEPROM.write(addr, char(mode));
        EEPROM.commit();

        return false;
    }

    std::map<String, String> LocalStorageConfigManager::ReadServerConfiguration()
    {
        std::map<String, String> configuration;

        if (FILE_SYSTEM.exists(filename))
        {
            File configFile = FILE_SYSTEM.open(filename, "r");
            if (configFile)
            {
                String configData;
                while (configFile.available())
                {
                    configData += char(configFile.read());
                }
                DynamicJsonDocument json(1024);
                auto deserializeError = deserializeJson(json, configData);
                serializeJson(json, Serial);
                if (!deserializeError)
                {	
                    // todo
                    // this is specific to mqtt. maybe find a way to loop through everything and provide a big map?
                    configuration.emplace("server", json["mqtt_server"]);
                    configuration.emplace("username", json["mqtt_user"]);
                    configuration.emplace("password", json["mqtt_pass"]);
                    configuration.emplace("topic", json["mqtt_topic"]);
                }
                else
                {
                    // todo - failure scenario
                }
                configFile.close();
            }
            else
            {
                // todo - failure scenario
            }
        }
        else
        {
            // todo - failure scenario
        }

        return configuration;
    }
    
    int LocalStorageConfigManager::WriteServerConfiguration(std::map<String, String> configuration)
    {
        return 0;
    }

    LocalStorageConfigManager::LocalStorageConfigManager()
    {
        EEPROM.begin(512);
        FILE_SYSTEM.begin();
    }
}