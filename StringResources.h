#ifndef StringResources_h
#define StringResources_h

#include <WString.h>

class OLEDStrings
{
    public:
        static inline const String StartupConfigPromptLine1 = "Press Button";
        static inline const String StartupConfigPromptLine2 = "Now for";
        static inline const String StartupConfigPromptLine3 = "Config Menu";

        static inline const String EnteringConfigLine1 = "Entering";
        static inline const String EnteringConfigLine2 = "Config Menu";
        static inline const String EnteringConfigLine3 = "";

        static inline const String StartupMessageLine1 = "Warming up";
        static inline const String StartupMessageLine2 = "Serial Number:";

        static inline const String ConfigSavedLine1 = "Saved";
        static inline const String ConfigSavedLine2 = "Release";
        static inline const String ConfigSavedLine3 = "Button Now";

        static inline const String RebootingLine1 = "Rebooting";
        static inline const String RebootingLine2 = "in";
        static inline const String RebootingLine3 = "5 seconds";

        static inline const String ConfigTempC = "Temp. in C";
        static inline const String ConfigTempF = "Temp. in F";
        static inline const String ConfigPMAQI = "PM in US AQI";
        static inline const String ConfigPMugm3 = "PM in ug/m3";
        static inline const String ConfigPMμgm3 = "PM in μg/m3";
        static inline const String ConfigSaveMessage = "Long Press Saves";

        static inline const String AQIAbbreviation = "AQI:";
        static inline const String CO2Abbreviation = "CO2:";
        static inline const String PMAbbreviation = "PM:";
        static inline const String TVOCAbbreviation = "TVOC:";
        static inline const String NOXAbbreviation = "NOX:";
};

class DebugMessages
{
    public:
        static inline const String HelloWorld = "Hello, world!";

        static inline const String ConfigButtonPinHigh = "currentState: High";
        static inline const String ConfigButtonPinLow = "currentState: Low";

        static inline const String SensirionSensor = "SHT:";
        static inline const String RealHumidityAbbreviation = "  RH: ";
        static inline const String TemperatureAbbreviation = "  T:  ";
        static inline const String SensirionSensorReadError = "Error in readSample()";

};

#endif