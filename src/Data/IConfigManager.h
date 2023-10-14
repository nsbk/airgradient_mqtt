#ifndef IConfigManager_h
#define IConfigManager_h

#include <WString.h>
#include <map>

namespace Data
{
    // This interface provides communication with data storage.
    class IConfigManager
    {
        public:
            virtual int ReadDisplayConfigurationMode() = 0;
            virtual bool WriteDisplayConfigurationMode(int mode) = 0;
            virtual std::map<String, String> ReadServerConfiguration() = 0;
            virtual int WriteServerConfiguration(std::map<String, String> configuration) = 0;
    };
}

#endif