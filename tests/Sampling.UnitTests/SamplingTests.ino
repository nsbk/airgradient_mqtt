#line 2 "SamplingTests.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <WString.h>
#include <Sampling.h>

using aunit::TestRunner;
using aunit::Verbosity;
using namespace Sampling;

test(SamplingTests, GetAQI)
{
    QualitySample airQuality = QualitySample();

    // None
    airQuality.PM25 = 0.0;
    assertEqual(0, airQuality.GetAQI());

    airQuality.PM25 = 11;
    assertEqual(46, airQuality.GetAQI());

    airQuality.PM25 = 12.0;
    assertEqual(50, airQuality.GetAQI());

    airQuality.PM25 = 33.2;
    assertEqual(95, airQuality.GetAQI());

    airQuality.PM25 = 35.4;
    assertEqual(100, airQuality.GetAQI());

    airQuality.PM25 = 47;
    assertEqual(129, airQuality.GetAQI());

    airQuality.PM25 = 55.4;
    assertEqual(150, airQuality.GetAQI());

    airQuality.PM25 = 127.3;
    assertEqual(188, airQuality.GetAQI());

    airQuality.PM25 = 150.4;
    assertEqual(200, airQuality.GetAQI());

    airQuality.PM25 = 240;
    assertEqual(290, airQuality.GetAQI());

    airQuality.PM25 = 250.4;
    assertEqual(300, airQuality.GetAQI());

    airQuality.PM25 = 350.4;
    assertEqual(400, airQuality.GetAQI());

    airQuality.PM25 = 500.4;
    assertEqual(500, airQuality.GetAQI());
}

test(SamplingTests, TempFConversion)
{
    // AUnit struggles with floats. Convert to string to get around the issue.
    QualitySample airQuality = QualitySample();
    String resultString;

    airQuality.TemperatureC = -40;
    resultString = String(airQuality.TemperatureF());
    assertEqual("-40.00", resultString);

    airQuality.TemperatureC = 0;
    resultString = String(airQuality.TemperatureF());
    assertEqual("32.00", resultString);

    airQuality.TemperatureC = 37;
    resultString = String(airQuality.TemperatureF());
    assertEqual("98.60", resultString);

    airQuality.TemperatureC = 100;
    resultString = String(airQuality.TemperatureF());
    assertEqual("212.00", resultString);
}

void setup()
{
#if ! defined(EPOXY_DUINO)
    delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif
    SERIAL_PORT_MONITOR.begin(115200);
    while (!SERIAL_PORT_MONITOR); // needed for Leonardo/Micro

    TestRunner::setVerbosity(Verbosity::kAll);
}

void loop()
{
    TestRunner::run();
}
