#ifndef QualitySample_h
#define QualitySample_h

#include <WString.h>

class QualitySample
{
    public:
        int CO2;
        int TVOC;
        int NOX;
        int PM1;
        float PM25;
        int PM10;
        float TemperatureC;
        int RelativeHumidity;

        QualitySample();
        float TemperatureF();
        int GetAQI();

        static inline const String CO2Units = "ppm";
        static inline const String ParticualteMatterUnits = "μg/m3";
        static inline const String RelativeHumidityUnits = "%";
        static inline const String TemperatureCUnits = "°C";
        static inline const String TemperatureFUnits = "°F";
};

#endif