#ifndef IConfigManager_h
#define IConfigManager_h

#include <WString.h>

namespace Data
{
    // This interface provides communication with data storage.
    class IConfigManager
    {
        public:
            virtual int ReadDisplayConfigurationMode()=0;
            virtual int WriteDisplayConfigurationMode(int mode)=0;

            // read write json data... array of chararrays? map?
    };
}

#endif