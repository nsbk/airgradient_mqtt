#line 2 "QualitySampleTest.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <WString.h>
//#include <QualitySample.h>
#include "../../src/Sampling/QualitySample.h"

using aunit::TestRunner;

test(SamplingTest, GetAQI)
{
    QualitySample airQuality = QualitySample();

    // None
    airQuality.PM25 = 0.0;
    assertEqual(airQuality.GetAQI(), 0);

    // Good
    airQuality.PM25 = 12.0;
    assertEqual(airQuality.GetAQI(), 50);

    // Moderate
    airQuality.PM25 = 35.4;
    assertEqual(airQuality.GetAQI(), 100);

    // Unhealthy for Sensitive Groups
    airQuality.PM25 = 55.4;
    assertNear(airQuality.GetAQI(), 150, 1);

    // Unhealthy
    airQuality.PM25 = 150.4;
    assertNear(airQuality.GetAQI(), 200, 1);

    // Very Unhealthy
    airQuality.PM25 = 250.4;
    assertNear(airQuality.GetAQI(), 300, 1);

    // Hazardous 
    airQuality.PM25 = 350.4;
    assertNear(airQuality.GetAQI(), 400, 1);

    // Hazardous 
    airQuality.PM25 = 500.4;
    assertNear(airQuality.GetAQI(), 500, 1);
}

test(SamplingTest, TempFConversion)
{
    // AUnit struggles with floats. Convert to string to get around the issue.
    QualitySample airQuality = QualitySample();
    String resultString;


    airQuality.TemperatureC = -40;
    resultString = String(airQuality.TemperatureF());
    assertEqual(resultString, "-40.00");

    airQuality.TemperatureC = 0;
    resultString = String(airQuality.TemperatureF());
    assertEqual(resultString, "32.00");

    airQuality.TemperatureC = 37;
    resultString = String(airQuality.TemperatureF());
    assertEqual(resultString, "98.60");

    airQuality.TemperatureC = 100;
    resultString = String(airQuality.TemperatureF());
    assertEqual(resultString, "212.00");
}

void setup()
{
#if ! defined(EPOXY_DUINO)
    delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif
    SERIAL_PORT_MONITOR.begin(115200);
    while (!SERIAL_PORT_MONITOR); // needed for Leonardo/Micro
}

void loop()
{
    TestRunner::run();
}
