#include "QualitySample.h"

QualitySample::QualitySample()
{
    CO2 = 0;
    TVOC = 0;
    NOX = 0;
    PM25 = 0.0;
    PM1 = 0;
    PM10 = 0;
    TemperatureC = 0;
    RelativeHumidity = 0;
}

float QualitySample::TemperatureF()
{
    return (TemperatureC * 9 / 5) + 32;
}

int QualitySample::GetAQI()
{
    /*
        From airnow.gov
        https://www.airnow.gov/sites/default/files/2020-05/aqi-technical-assistance-document-sept2018.pdf

        Ip = (IHi - ILo) / (BPHi - BPLo) * (Cp - BPLo) + ILo

        Ip = Index for Pollutant p (the AQI result)
        Cp = the concentration of the pullutant (μg/m3)
        BPHi = the concentration breakpoint greater than or equal to Cp
        BPLo = the concentration breakpoint less than or equal to Cp
        IHi = the AQI value corresponding to BPHi
        ILo = the AQI value corresponding to BPLo
    */

    // Good
    if (PM25 <= 12.0) 
        return ((50 - 0) / (12.0 - .0) * (PM25 - .0) + 0);
    // Moderate
    else if (PM25 <= 35.4) 
        return ((100 - 50) / (35.4 - 12.0) * (PM25 - 12.0) + 50);
    // Unhealthy for Sensitive Groups
    else if (PM25 <= 55.4) 
        return ((150 - 100) / (55.4 - 35.4) * (PM25 - 35.4) + 100);
    // Unhealthy
    else if (PM25 <= 150.4) 
        return ((200 - 150) / (150.4 - 55.4) * (PM25 - 55.4) + 150);
    // Very Unhealthy
    else if (PM25 <= 250.4) 
        return ((300 - 200) / (250.4 - 150.4) * (PM25 - 150.4) + 200);
    // Hazardous  
    else if (PM25 <= 350.4) 
        return ((400 - 300) / (350.4 - 250.4) * (PM25 - 250.4) + 300);
    // Hazardous
    else if (PM25 <= 500.4) 
        return ((500 - 400) / (500.4 - 350.4) * (PM25 - 350.4) + 400);
    else 
        return 500;
}