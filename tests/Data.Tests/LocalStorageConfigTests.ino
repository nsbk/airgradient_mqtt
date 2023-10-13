#line 2 "LocalStorageConfigTests.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <Data.h>
#include <LocalStorageConfigManager.h>

using aunit::TestRunner;
using aunit::Verbosity;
using namespace Data;
using namespace Data::Local;

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
