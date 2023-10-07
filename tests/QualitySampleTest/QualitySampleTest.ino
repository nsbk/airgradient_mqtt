#line 2 "QualitySampleTest.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <QualitySample.h>

using aunit::TestRunner;

test(SamplingTest, GetAQI_Valid)
{
    QualitySample airQuality = QualitySample();

    airQuality.PM25 = 12;
    assertEqual(airQuality.GetAQI(), 50);

    airQuality.PM25 = 35.4;
    assertNear(airQuality.GetAQI(), 100, 1);

    airQuality.PM25 = 55.4;
    assertNear(airQuality.GetAQI(), 150, 1);

    airQuality.PM25 = 150.4;
    assertNear(airQuality.GetAQI(), 200, 1);

    airQuality.PM25 = 250.4;
    assertNear(airQuality.GetAQI(), 300, 1);

    airQuality.PM25 = 350.4;
    assertNear(airQuality.GetAQI(), 400, 1);

    airQuality.PM25 = 500.4;
    assertNear(airQuality.GetAQI(), 500, 1);
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
