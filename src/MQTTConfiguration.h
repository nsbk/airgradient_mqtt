#ifndef MQTTConfiguration_h
#define MQTTConfiguration_h

#include <WString.h>

// PubSubClient expects char arrays
class MQTTConfiguration
{
  public:
    char Server[40];
    char User[40];
    char Password[40];
    char Topic[80];  
};

#endif