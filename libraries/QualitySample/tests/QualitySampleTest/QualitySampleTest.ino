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
