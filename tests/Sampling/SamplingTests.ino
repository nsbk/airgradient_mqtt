#include <AUnit.h>
#include "../../src/StringResources.h"
#include "../../src/Sampling/SampleInterval.h"
#include "../../src/Sampling/QualitySample.h"


QualitySample airQuality = QualitySample();




void setup()
{

#if ! defined(EPOXY_DUINO)
    delay(1000); // Wait for stability on some boards, otherwise garage on Serial
#endif
    Serial.begin(115200); // ESP8266 default 74880 not supported on Linux
    while (!Serial); // for the Arduino Leonardo/Micro only
}

// Call the test runner in loop
void loop() {
    aunit::TestRunner::run();
}
